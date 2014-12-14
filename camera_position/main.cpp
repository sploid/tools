#include "stdafx.h"


#include <opencv2/calib3d/calib3d_c.h>
#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "opencv2/opencv.hpp"
#include "depth_widget.h"

#define FOCAL_LENGTH 1000

bool search_symbol(int sym_index, const zbar::Image& zbar_image, CvPoint2D32f* point) {
  for (auto it = zbar_image.symbol_begin(); it != zbar_image.symbol_end(); ++it ) {
    if (it->get_data() == std::to_string(sym_index + 1)) {
      std::vector<cv::Point> vp;
		  const int count_locations = it->get_location_size();
      if (count_locations != 4) {
        std::cout << "Invalid location";
        return false;
      }
		  for (int nn = 0; nn < count_locations; ++nn) {
        vp.push_back(cv::Point(it->get_location_x(nn), it->get_location_y(nn)));
		  }
      const cv::RotatedRect r = cv::minAreaRect(vp);
      point->x = r.center.x;
      point->y = r.center.y;
      return true;
    }
  }
  return false;
}

bool read_points(const cv::Mat& image, CvPoint2D32f points_in_image[4]) {
  zbar::Image zbar_image(image.cols, image.rows, "Y800", static_cast<uchar*>(image.data), image.cols * image.rows);
  zbar::ImageScanner scanner;
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
  scanner.scan(zbar_image);
  for (int nn = 0; nn < 4; ++nn) {
    if (!search_symbol(nn, zbar_image, &points_in_image[nn])) {
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  QVTKApplication app(argc, argv);
  const QStringList args = QCoreApplication::arguments();
  if (args.size() <= 1) {
    std::cout << "Missing input file";
  }

  const cv::Mat input = cv::imread(args.at(1).toLocal8Bit().data(), CV_LOAD_IMAGE_GRAYSCALE);
  CvPoint2D32f points_in_image[4];
  if (read_points(input, points_in_image)) {
    // The calibration matrix
    cv::Mat _A_matrix = cv::Mat::zeros(3, 3, CV_64FC1);   // intrinsic camera parameters
    // Intrinsic camera parameters: UVC WEBCAM
    double f = 55;                           // focal length in mm
    double sx = 22.3, sy = 14.9;             // sensor size
    double width = 640, height = 480;        // image size

    double params_WEBCAM[] = {  width*f/sx,   // fx
                                height*f/sy,  // fy
                                width/2,      // cx
                                height/2};    // cy

    _A_matrix.at<double>(0, 0) = params_WEBCAM[0];       //      [ fx   0  cx ]
    _A_matrix.at<double>(1, 1) = params_WEBCAM[1];       //      [  0  fy  cy ]
    _A_matrix.at<double>(0, 2) = params_WEBCAM[2];       //      [  0   0   1 ]
    _A_matrix.at<double>(1, 2) = params_WEBCAM[3];
    _A_matrix.at<double>(2, 2) = 1;

    float cubeSize = 10.0;
    std::vector<cv::Point3f> modelPoints;
    modelPoints.push_back(cvPoint3D32f(0.0f, 0.0f, 0.0f)); //The first must be (0,0,0)
    modelPoints.push_back(cvPoint3D32f(cubeSize, 0.0f, 0.0f));
    modelPoints.push_back(cvPoint3D32f(cubeSize, cubeSize, 0.0f));
    modelPoints.push_back(cvPoint3D32f(0.0f, cubeSize, 0.0f));

    std::vector<cv::Point2f> list_points2d;
    for ( int nn = 0; nn < 4; ++nn ) {
      list_points2d.push_back(cv::Point2f(points_in_image[nn].x, points_in_image[nn].y));
    }
    cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64FC1);  // vector of distortion coefficients
    cv::Mat rvec = cv::Mat::zeros(3, 1, CV_64FC1);          // output rotation vector
    cv::Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);    // output translation vector

    cv::solvePnPRansac(modelPoints, list_points2d, _A_matrix, distCoeffs, rvec, tvec);
    cv::Mat _R_matrix = cv::Mat::zeros(3, 3, CV_64FC1); // rotation matrix
    cv::Rodrigues(rvec, _R_matrix);      // converts Rotation Vector to Matrix
    cv::Mat _t_matrix = tvec; // translation matrix
    cv::Mat _P_matrix = cv::Mat::zeros(3, 4, CV_64FC1);   // rotation-translation matrix
    // Rotation-Translation Matrix Definition
    _P_matrix.at<double>(0,0) = _R_matrix.at<double>(0,0);
    _P_matrix.at<double>(0,1) = _R_matrix.at<double>(0,1);
    _P_matrix.at<double>(0,2) = _R_matrix.at<double>(0,2);
    _P_matrix.at<double>(1,0) = _R_matrix.at<double>(1,0);
    _P_matrix.at<double>(1,1) = _R_matrix.at<double>(1,1);
    _P_matrix.at<double>(1,2) = _R_matrix.at<double>(1,2);
    _P_matrix.at<double>(2,0) = _R_matrix.at<double>(2,0);
    _P_matrix.at<double>(2,1) = _R_matrix.at<double>(2,1);
    _P_matrix.at<double>(0,3) = _t_matrix.at<double>(0);
    _P_matrix.at<double>(1,3) = _t_matrix.at<double>(1);
    _P_matrix.at<double>(2,3) = _t_matrix.at<double>(2);

    cv::transform(modelPoints, _P_matrix, _P_matrix);

/*    float cubeSize = 10.0;
    std::vector<CvPoint3D32f> modelPoints;
    modelPoints.push_back(cvPoint3D32f(0.0f, 0.0f, 0.0f)); //The first must be (0,0,0)
    modelPoints.push_back(cvPoint3D32f(cubeSize, 0.0f, 0.0f));
    modelPoints.push_back(cvPoint3D32f(cubeSize, cubeSize, 0.0f));
    modelPoints.push_back(cvPoint3D32f(0.0f, cubeSize, 0.0f));

    CvPOSITObject* posit_object = cvCreatePOSITObject(&modelPoints[0], static_cast<int>(modelPoints.size()));
    assert(posit_object);

  	//Estimate the pose
	  float rotation_matrix[9];
	  float translation_vector[3];	
	  CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 1.0e-4f);
	  cvPOSIT( posit_object, &points_in_image[0], FOCAL_LENGTH, criteria, &rotation_matrix[0], &translation_vector[0] );

    //Project the model points with the estimated pose
	  std::vector<CvPoint2D32f> projectedPoints;
	  for ( size_t  p = 0; p < modelPoints.size(); p++ ) {
		  CvPoint3D32f point3D;
		  point3D.x = rotation_matrix[0] * modelPoints[p].x + rotation_matrix[1] * modelPoints[p].y + rotation_matrix[2] * modelPoints[p].z + translation_vector[0];
		  point3D.y = rotation_matrix[3] * modelPoints[p].x + rotation_matrix[4] * modelPoints[p].y + rotation_matrix[5] * modelPoints[p].z + translation_vector[1];
		  point3D.z = rotation_matrix[6] * modelPoints[p].x + rotation_matrix[7] * modelPoints[p].y + rotation_matrix[8] * modelPoints[p].z + translation_vector[2];
		  CvPoint2D32f point2D = cvPoint2D32f( 0.0, 0.0 );
		  if ( point3D.z != 0 )
		  {
			  point2D.x = FOCAL_LENGTH * point3D.x / point3D.z; 
			  point2D.y = FOCAL_LENGTH * point3D.y / point3D.z;	
		  }
		  projectedPoints.push_back( point2D );
	  }
    int t = 0;*/
  }
	const int ret = app.exec();

	return ret;
}

