#include "popt_pp.h"
#include "jpp.h"

int main(int argc, const char** argv) {
  const char* left_img_file;
  const char* right_img_file;
  const char* calib_file;
  const char* jpp_config_file;
  const char* img_dir;
  const char* output = "astar";
  int w = 0;
  int v = 0;
  int num_imgs = 0;
  
  clock_t start, end;
  
  static struct poptOption options[] = {
    { "num_imgs",'n',POPT_ARG_INT,&num_imgs,0,"Number of images to be processed","NUM" },
    { "img_dir",'d',POPT_ARG_STRING,&img_dir,0,"Directory containing image pairs (set if n > 0)","STR" },
    { "left_img",'l',POPT_ARG_STRING,&left_img_file,0,"Left image file name","STR" },
    { "right_img",'r',POPT_ARG_STRING,&right_img_file,0,"Right image file name","STR" },
    { "calib_file",'c',POPT_ARG_STRING,&calib_file,0,"Stereo calibration file name","STR" },
    { "jpp_config_file",'j',POPT_ARG_STRING,&jpp_config_file,0,"JPP config file name","STR" },
    { "output",'o',POPT_ARG_STRING,&output,0,"Output - astar, rrt, debug","STR" },
    { "visualize",'v',POPT_ARG_INT,&v,0,"Set v=1 for displaying visualizations","NUM" },
    { "write_files",'w',POPT_ARG_INT,&w,0,"Set w=1 for writing visualizations to files","NUM" },
    POPT_AUTOHELP
    { NULL, 0, 0, NULL, 0, NULL, NULL }
  };

  POpt popt(NULL, argc, argv, options, 0);
  int c;
  while((c = popt.getNextOpt()) >= 0) {}
  
  // Read JPP config
  config_t cfg, *cf;
  cf = &cfg;
  config_init(cf);
  if (!config_read_file(cf, jpp_config_file)) {
    cout << "Could not read config file!" << endl;
    config_destroy(cf);
    return(EXIT_FAILURE);
  }
  
  // Read stereo calibration info
  FileStorage fs = FileStorage(calib_file, FileStorage::READ);
  
  // JPP instance
  JPP jpp(fs, cf);
  
  if (num_imgs == 0) {
    Mat img_left = imread(left_img_file, CV_LOAD_IMAGE_COLOR);
    Mat img_right = imread(right_img_file, CV_LOAD_IMAGE_COLOR);
    
    char dmap_file[100], rrt_file_prefix[100], astar_file_prefix[100];
    sprintf(astar_file_prefix, "%s%d", "astar", 0);
    sprintf(rrt_file_prefix, "%s%d", "rrt", 0);
    sprintf(dmap_file, "%s%d.jpg", "dmap", 0);
    
    jpp.load_images(img_left, img_right);
    
    if (strcmp(output, "astar") == 0) {
      vector< Point > path = jpp.plan_astar();
      if (v == 1) {
        pair< Mat, Mat > vis;
        if (w == 1)
          vis = jpp.visualize_jpp(astar_file_prefix);
        else
          vis = jpp.visualize_jpp();
        while (1) {
          imshow("PATH", vis.first);
          imshow("CONFIDENCE", vis.second);
          if (waitKey(30) > 0)
            break;
        }
      }
    } else if (strcmp(output, "rrt") == 0) {
      vector< Point > path = jpp.plan_rrt();
      if (v == 1) {
        pair< Mat, Mat > vis;
        if (w == 1)
          vis = jpp.visualize_jpp(rrt_file_prefix);
        else
          vis = jpp.visualize_jpp();
        while (1) {
          imshow("PATH", vis.first);
          imshow("CONFIDENCE", vis.second);
          if (waitKey(30) > 0)
            break;
        }
      }
    } else if (strcmp(output, "debug") == 0) {
      Mat conf_pos = jpp.visualize_conf_pos();
      Mat conf_neg = jpp.visualize_conf_neg();
      while (1) {
        imshow("CONF POS", conf_pos);
        imshow("CONF NEG", conf_neg);
        if (waitKey(30) > 0) {
          break;
        }
      }
    }
  } else if (num_imgs > 0) {
    double total_time = 0;
    for (int i = 1; i <= num_imgs; i++) {
      cout << "Processing pair " << i << endl;
      char left_img_file[100], right_img_file[100];
      char dmap_file[100], rrt_file_prefix[100], astar_file_prefix[100];
      sprintf(left_img_file, "%s%s%d.jpg", img_dir, "left", i);
      sprintf(right_img_file, "%s%s%d.jpg", img_dir, "right", i);
      sprintf(dmap_file, "%s%d.jpg", "dmap", i);
      sprintf(astar_file_prefix, "%s%d", "astar", i);
      sprintf(rrt_file_prefix, "%s%d", "rrt", i);
      
      Mat img_left = imread(left_img_file, CV_LOAD_IMAGE_COLOR);
      Mat img_right = imread(right_img_file, CV_LOAD_IMAGE_COLOR);
      
      start = clock();
      jpp.load_images(img_left, img_right);
      
      if (strcmp(output, "astar") == 0) {
        vector< Point > path = jpp.plan_astar();
        if (v == 1) {
          pair< Mat, Mat > vis;
          if (w == 1)
            vis = jpp.visualize_jpp(astar_file_prefix);
          else
            vis = jpp.visualize_jpp();
          while (1) {
            imshow("PATH", vis.first);
            imshow("CONFIDENCE", vis.second);
            if (waitKey(30) > 0)
              break;
          }
        }
      } else if (strcmp(output, "rrt") == 0) {
        vector< Point > path = jpp.plan_rrt();
        if (v == 1) {
          pair< Mat, Mat > vis;
          if (w == 1)
            vis = jpp.visualize_jpp(rrt_file_prefix);
          else
            vis = jpp.visualize_jpp();
          while (1) {
            imshow("PATH", vis.first);
            imshow("CONFIDENCE", vis.second);
            if (waitKey(30) > 0)
              break;
          }
        }
      } else if (strcmp(output, "debug") == 0) {
        Mat conf_pos = jpp.visualize_conf_pos();
        Mat conf_neg = jpp.visualize_conf_neg();
        while (1) {
          imshow("CONF POS", conf_pos);
          imshow("CONF NEG", conf_neg);
          if (waitKey(30) > 0) {
            break;
          }
        }
      }
      end = clock();
      double secs = ((double) (end - start)) / CLOCKS_PER_SEC;
      total_time += secs;
      cout << "Time: " << secs << endl;
    }
    cout << "Avg time: " << (total_time/(double)num_imgs) << endl; 
  }
  
  config_destroy(cf);
  return 0;
}
