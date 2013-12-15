cmake_minimum_required(VERSION 2.8)

project( MyGestureRecognition )
   find_package( OpenCV REQUIRED )
   add_executable( MyGestureRecognition MyGestureRecognition )
   target_link_libraries( MyGestureRecognition ${OpenCV_LIBS} )
