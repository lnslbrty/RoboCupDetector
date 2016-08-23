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
      cv::Mat out = ws->getImage(i);
      if (!out.empty()) {
         std::vector<uchar> buf;
         if (cv::imencode(".jpg", out, buf, ws->getFlags()) == true) {
           response = MHD_create_response_from_buffer(
                        buf.size(),
                        (void*) &buf[0],
                        MHD_RESPMEM_MUST_COPY);
           ret = MHD_queue_response(connection,
                   MHD_HTTP_OK,
                   response);
           MHD_destroy_response(response);
           return ret;
         }
      }
    }
    return MHD_NO;
  }

  std::stringstream tmp;
  tmp << "<html>\n"
      << "<head>\n"
      << "<meta http-equiv=\"Cache-Control\" content=\"no-cache; must-revalidate\">\n"
      << "<script language=\"JavaScript\" type=\"text/javascript\">\n"
      << "refreshImage = function()\n{\n";
  for (size_t i = 0; i < ws->getCount(); ++i) {
    if (!ws->getImage(i).empty())
      tmp << "img = document.getElementById(\"img"<<i<<"\");\nimg.src=\"/cam"<<i<<".jpg?rand=\" + Math.random();\n";
  }
  tmp << "}\n</script>\n"
      << "<meta http-equiv=\"Cache-Control\" content=\"no-cache; must-revalidate\">\n"
      << "<meta http-equiv=\"Content-Type\" content=\"text/html\">\n"
      << "<title>robocup opencv viewer</title>\n"
      << "</head>\n\n"
      << "<body onload=\"window.setInterval(refreshImage, 1*1000);\"><b>RoboCup OpenCV Viewer</b> ... frame #"<<ws->getFrames()<<"<br>\n";
  for (size_t i = 0; i < ws->getCount(); ++i) {
    if (!ws->getImage(i).empty())
      tmp << "<img src=\"/cam" <<i<< ".jpg\" id=\"img"<<i<<"\"><br><br>\n";
  }
  tmp << "</body></html>\n";
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
  httpd = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY,
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

void rc::WebServer::stop(void) {
  MHD_stop_daemon(httpd);
  delete[] images_out;
}
