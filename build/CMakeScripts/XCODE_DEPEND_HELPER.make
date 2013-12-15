# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# For each target create a dummy rule so the target does not have to exist
/usr/local/lib/libopencv_videostab.2.4.7.dylib:
/usr/local/lib/libopencv_video.2.4.7.dylib:
/usr/local/lib/libopencv_ts.a:
/usr/local/lib/libopencv_superres.2.4.7.dylib:
/usr/local/lib/libopencv_stitching.2.4.7.dylib:
/usr/local/lib/libopencv_photo.2.4.7.dylib:
/usr/local/lib/libopencv_ocl.2.4.7.dylib:
/usr/local/lib/libopencv_objdetect.2.4.7.dylib:
/usr/local/lib/libopencv_nonfree.2.4.7.dylib:
/usr/local/lib/libopencv_ml.2.4.7.dylib:
/usr/local/lib/libopencv_legacy.2.4.7.dylib:
/usr/local/lib/libopencv_imgproc.2.4.7.dylib:
/usr/local/lib/libopencv_highgui.2.4.7.dylib:
/usr/local/lib/libopencv_gpu.2.4.7.dylib:
/usr/local/lib/libopencv_flann.2.4.7.dylib:
/usr/local/lib/libopencv_features2d.2.4.7.dylib:
/usr/local/lib/libopencv_core.2.4.7.dylib:
/usr/local/lib/libopencv_contrib.2.4.7.dylib:
/usr/local/lib/libopencv_calib3d.2.4.7.dylib:


# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.MyGestureRecognition.Debug:
/Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/Debug/MyGestureRecognition:\
	/usr/local/lib/libopencv_videostab.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ts.a\
	/usr/local/lib/libopencv_superres.2.4.7.dylib\
	/usr/local/lib/libopencv_stitching.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib\
	/usr/local/lib/libopencv_contrib.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib
	/bin/rm -f /Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/Debug/MyGestureRecognition


PostBuild.MyGestureRecognition.Release:
/Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/Release/MyGestureRecognition:\
	/usr/local/lib/libopencv_videostab.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ts.a\
	/usr/local/lib/libopencv_superres.2.4.7.dylib\
	/usr/local/lib/libopencv_stitching.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib\
	/usr/local/lib/libopencv_contrib.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib
	/bin/rm -f /Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/Release/MyGestureRecognition


PostBuild.MyGestureRecognition.MinSizeRel:
/Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/MinSizeRel/MyGestureRecognition:\
	/usr/local/lib/libopencv_videostab.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ts.a\
	/usr/local/lib/libopencv_superres.2.4.7.dylib\
	/usr/local/lib/libopencv_stitching.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib\
	/usr/local/lib/libopencv_contrib.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib
	/bin/rm -f /Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/MinSizeRel/MyGestureRecognition


PostBuild.MyGestureRecognition.RelWithDebInfo:
/Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/RelWithDebInfo/MyGestureRecognition:\
	/usr/local/lib/libopencv_videostab.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ts.a\
	/usr/local/lib/libopencv_superres.2.4.7.dylib\
	/usr/local/lib/libopencv_stitching.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib\
	/usr/local/lib/libopencv_contrib.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_nonfree.2.4.7.dylib\
	/usr/local/lib/libopencv_ocl.2.4.7.dylib\
	/usr/local/lib/libopencv_gpu.2.4.7.dylib\
	/usr/local/lib/libopencv_photo.2.4.7.dylib\
	/usr/local/lib/libopencv_objdetect.2.4.7.dylib\
	/usr/local/lib/libopencv_legacy.2.4.7.dylib\
	/usr/local/lib/libopencv_video.2.4.7.dylib\
	/usr/local/lib/libopencv_ml.2.4.7.dylib\
	/usr/local/lib/libopencv_calib3d.2.4.7.dylib\
	/usr/local/lib/libopencv_features2d.2.4.7.dylib\
	/usr/local/lib/libopencv_highgui.2.4.7.dylib\
	/usr/local/lib/libopencv_imgproc.2.4.7.dylib\
	/usr/local/lib/libopencv_flann.2.4.7.dylib\
	/usr/local/lib/libopencv_core.2.4.7.dylib
	/bin/rm -f /Users/yeahugo/Documents/Search/Computer\ Vision/MyGestureRecognition/build/RelWithDebInfo/MyGestureRecognition


