#include "RC_WebServer.hpp"

#include <iostream>
#include <sstream>


static int httpd_callback(void * cls,
                    struct MHD_Connection * connection,
                    const char * url,
                    const char * method,
                    const char * version,
                    const char * upload_data,
                    size_t * upload_data_size,
                    void ** ptr) {
  rc::WebServer * ws = (rc::WebServer *) cls;
  struct MHD_Response * response;
  int ret;

  if (0 != strcmp(method, "GET"))
    return MHD_NO; /* unexpected method */
  if (0 != *upload_data_size)
    return MHD_NO;

  std::string surl(url);
  size_t uend;
  if ( (surl.find("/cam")) == 0
    && (uend = surl.find(".")) > 0 ) {
    size_t i;
    try {
      i = std::stoi(surl.substr(4,uend-4));
    } catch (std::invalid_argument& ia) {
      return MHD_NO;
    }
    if (i >= 0 && i < ws->getCount()) {
      std::cout <<"BLA"<<i<<std::endl;
    }
  }

  std::stringstream tmp;
  tmp << "<html>\n"
      << "<title>robocup opencv viewer</title>\n"
      << "<b>RoboCup OpenCV Viewer</b><br>\n";
  for (size_t i = 0; i < ws->getCount(); ++i) {
    tmp << "<img href=\"/cam" <<i<< ".jpg\"><br>\n";
  }
  tmp << "</html>\n";
  std::string output = tmp.str();
  response = MHD_create_response_from_buffer(
               output.size(),
               (void*) output.c_str(),
               MHD_RESPMEM_MUST_COPY);
  ret = MHD_queue_response(connection,
                           MHD_HTTP_OK,
                           response);
  MHD_destroy_response(response);
  return ret;
}

bool rc::WebServer::start() {
  images_out = new cv::Mat[image_cnt];
  for (size_t i = 0; i < image_cnt; ++i) {
    images_out[i] = cv::Mat::zeros(1, 1, CV_32F);
  }
  httpd = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
    port,
    NULL,
    NULL,
    &httpd_callback,
    (this),
    MHD_OPTION_END);
  if (httpd == NULL)
    return false;
  return true;
}
