/**
 * @file test_detector.cpp
 * @author Jun Jin
 * @date 3 Mar 2017
 * @brief Implementation for the ArcDetector class
 *
 */
#include <au_vision/arc_detector.h>
extern "C"{
#include "au_vision/external/image.h"
#include "au_vision/external/pgm.h"
#include "au_vision/external/misc.h"
#include "au_vision/external/svg.h"
#include "au_vision/external/polygon.h"
#include "au_vision/external/ring.h"
#include "au_vision/external/elsdc.h"
}


namespace au_vision
{
	ArcDetector::ArcDetector(ros::NodeHandle &nh, ros::NodeHandle &private_nh)
		: Detector( nh, private_nh )
	{
		detectorType_ = "arc_detector";
	}

ArcDetector::~ArcDetector()
{

}


std::vector<au_core::Roi> ArcDetector::detect(const cv::Mat &frame)
{
    std::vector<au_core::Roi> roiArray;
    /** only used for debugging
    //initialize the ROS system and become a node for publishing test results.
    //ros::init(argc, argv, "arvp_detection");// debugging test node only
    //ros::NodeHandle nh;

    //create a publisher object.
    //ros::Publisher pub = nh.advertise <std_msgs::String>("/horiz/camera/detection_result", 1000);
    //loop at 100Hz unit the node is shutdown. actual rate is determined by computation speed.
    //ros::Rate rosrate(100);
    **/
    //declaration of function
     PImageDouble read_pgm_image_double1(cv::Mat cv_image );
    //marks of the color space effective or not.
     bool isEffective=false;
     bool lockHLSS=false;

		 std::stringstream ss;
           /** only used for debugging
	   //create and fill in the message.
	   std_msgs::String msg;

           //calculate duration, for debugging only.
	   double e1=cv::getTickCount();
           **/

	   cv::Mat Src_L;
	   /// Convert image to gray and blur it
	   cv::Mat ViewControl=frame;
	   bool usingGray2View=false;//true: using gray image or HSL(L) image to view the result;  false: using RGB image to view the result
	   double BigCompareScale=0.35;//detection of the ellipse size, compared with the image's total width.


	   //decide to choose which color space.
	   //Src_L is used for final detection
	   if(isEffective||lockHLSS)
		 {
		   //convert image to Gray image. store in Src_L
			cv::cvtColor( frame, Src_L, CV_RGB2GRAY );
			if(usingGray2View)
				ViewControl=Src_L;
			//text only for debugging purpose
			//for debugging cv::putText(ViewControl,"RGB2GRAY",cv::Point(300,50),cv::FONT_HERSHEY_COMPLEX_SMALL,0.8,cv::Scalar(0,0,255),1);
		 }
		else if(!lockHLSS)
		{
			//convert from RGB to HLS, store in src_HLS
			cv::cvtColor( frame, src_HLS, CV_RGB2HLS );
			std::vector <cv::Mat> channels(3);//store HLS channels
			//split the channels
			cv::split(src_HLS, channels);
			Src_L=channels[2];//Src_L = channel S in HLS
			if(usingGray2View)
			ViewControl=Src_L;
		 	//text only for debugging purpose
		 	//for debugging cv::putText(ViewControl,"HLS-S",cv::Point(300,50),cv::FONT_HERSHEY_COMPLEX_SMALL,0.8,cv::Scalar(0,0,255),1);
		}

		cv::blur(Src_L, Src_L, cv::Size(3,3));//blur the image firstly, can be removed for test purpose.

		PImageDouble in;     /* input image */
		  PImageInt    out;    /* output image having the same size as 'in'; the pixels
								  supporting a certain geometric primitive are marked
								  with the same label */

		  int ell_count = 0;   /* number of detected ellipses */
		  int *ell_labels=NULL;/* the pixels supporting a certain ellipse are marked
								  with the same unique label */
		  Ring *ell_out = NULL;/* array containing the parameters of the detected
								  ellipses; correlated with ell_labels, i.e. the i-th
								  element of ell_labels is the label of the pixels
								  supporting the ellipse defined by the parameters
								  ell[i] */

		  int poly_count = 0;  /* number of detected polygons */
		  int *poly_labels=NULL;/* the pixels supporting a certain polygon are marked
								  with the same unique label */
		  Polygon *poly_out=NULL;/* array containing the parameters of the detected
								  polygons; correlated with poly_labels, i.e. the i-th
								  element of ell_labels is the label of the pixels
								  supporting the polygon defined by the parameters
								  poly[i] */

		  FILE *ell_ascii;     /* output file with the parameters of the detected
								  ellipses -- ASCII format */
		  FILE *poly_ascii;    /* output file with the parameters of the detected
								  polygons -- ASCII format */
		  FILE *fsvg;          /* output file with the detected ellipses and polygons
								  in vectorial form */
		  int i,j;

		  /* read input image; must be PGM form */
		  in = read_pgm_image_double1(Src_L);
		  int xsize = in->xsize, ysize = in->ysize;

		  /* create and initialize with 0 output label image */
		  out = new_PImageInt_ini( in->xsize, in->ysize, 0 );

		  /* call detection procedure */
		  ELSDc( in, &ell_count, &ell_out, &ell_labels, &poly_count, &poly_out,
				 &poly_labels, out );

		  /** for debugging purpose only
	      double e2=cv::getTickCount();
	      stringstream ssElls, ssPolys;
	      ssElls << ell_count;
		  ssPolys << poly_count;
	      std::string ellStr="Number of arcs detected = "+ssElls.str();
		  std::string polyStr="Number of segments detected = "+ssPolys.str();
		 double e3_debug=e2-e1;
		 double frequency=cv::getTickFrequency();
		 double durations=(e3_debug)/frequency;
		 std::ostringstream doubleStreams;
		 doubleStreams<<durations;
		 std::string fpsStr=doubleStreams.str()+"s";
		 cv::putText(ViewControl,	ellStr,	cv::Point(6,65),cv::FONT_HERSHEY_COMPLEX_SMALL,0.8,cv::Scalar(0,0,255),1);
		 cv::putText(ViewControl,	polyStr,cv::Point(6,85),cv::FONT_HERSHEY_COMPLEX_SMALL,0.8,cv::Scalar(0,0,255),1);
		 cv::putText(ViewControl,	fpsStr,cv::Point(6,105),cv::FONT_HERSHEY_COMPLEX_SMALL,0.8,cv::Scalar(0,0,255),1);
		 **/
                 //mark ellipse on the images
		 if( ell_out != NULL)
		{
			//for debugging if( (ell_ascii = fopen("out_ellipse.txt","w")) == NULL )//for debugging only
						//error("main: can't open ellipse output file.");
			 //for debugging fsvg = init_svg( "output.svg", xsize, ysize );//for debugging only

			 std::vector<int> CandidatesIndexs;
			 double MaxABValue=0;
			 for( i=0; i<ell_count; i++ )//fill the candidates
			 {
				 double startAngle=ell_out[i].ang_start;
				 double endAngle=ell_out[i].ang_end;
				 double RadiusTempt=(ell_out[i].ax+ell_out[i].bx)/2;
				 if(endAngle>=(startAngle+1.8) ||((endAngle<startAngle)&& (endAngle+6.2832-startAngle)>1.8))
				 {
					double isValid=false;
					//Detect the reflection Cases.
					if(startAngle>=1.57&&startAngle<=4.712)
						isValid=true;
					else if(startAngle>=0&&startAngle<1.57)
					{
						if(endAngle<startAngle||endAngle>4.712)
							isValid=true;
					}
					else if(startAngle>4.712)
					{
						if(endAngle>startAngle||endAngle<0.78)
							isValid=true;
					}
					//End of detecting the reflection cases
					double frameWidth=frame.cols;
					if(isEffective&&(RadiusTempt>=0.25*frameWidth||RadiusTempt<=0.02*frameWidth)) //Radius should not be too large
						isValid=false;
					if(RadiusTempt>=0.04*frameWidth)
						lockHLSS=true;

					if(isValid)
					{
						 //capare the a, and b values
						if(RadiusTempt>MaxABValue)
							MaxABValue= RadiusTempt;
						CandidatesIndexs.push_back(i);
						int thisIndex=i;
					}
				 }//end of if (endAngle>=
			 }//end of for( i=0; i<ell_count; i++ )//fill the candidates

			//fclose(ell_ascii);//for debugging only
			//fclose_svg( fsvg );//for debugging only

			isEffective=false;
			 for( i=0; i<CandidatesIndexs.size(); i++ )
			  {
				int thisIndex=CandidatesIndexs[i];
				 double thisABValue=(ell_out[thisIndex].ax+ell_out[thisIndex].bx)/2;
				 if(thisABValue>=BigCompareScale*MaxABValue)
				 {
					 //found the ROIs.
					 isEffective=true;

					 cv::Point2d startP, endP, centerP, centerStartP, centerEndP;
					 startP.x=ell_out[thisIndex].x1;
					 startP.y=ell_out[thisIndex].y1;
					endP.x=ell_out[thisIndex].x2;
					endP.y=ell_out[thisIndex].y2;
					centerP.x=ell_out[thisIndex].cx;
					centerP.y=ell_out[thisIndex].cy;
					centerStartP.x=centerP.x-3;
					centerStartP.y=centerP.y-3;
					centerEndP.x=centerP.x+6;
					centerEndP.y=centerP.y+6;
					//for debugging only
           double xoffset=centerStartP.x;
           if(xoffset>centerEndP.x)
              xoffset=centerEndP.x;
          double yoffset=centerStartP.y;
          if(yoffset>centerEndP.y)
              yoffset=centerEndP.y;
          double t_width=std::abs(centerEndP.x-centerStartP.x);
          double t_height=std::abs(centerEndP.y-centerStartP.y);
					//for debugging cv::rectangle(ViewControl,centerStartP,centerEndP,cv::Scalar(0,0,255),2);

					ss << "buoy,";
					//decide the color, using HLS hue channel
					int cx=(int)(centerP.x);
					int cy=(int)(centerP.y);
					int B_value=0;//(frame.at<cv::Vec3b>(cv::Point(cx,cy)))[0];
					int G_value=0;//(frame.at<cv::Vec3b>(cv::Point(cx,cy)))[1];
					int R_value=0;//(frame.at<cv::Vec3b>(cv::Point(cx,cy)))[2];
//					for(int p=0;p<5;p++)
//						for(int q=0;q<5;q++)
//					      {
//							B_value+=(frame.at<cv::Vec3b>(cv::Point(cx-2+p,cy-2+q)))[0];
//							G_value+=(frame.at<cv::Vec3b>(cv::Point(cx-2+p,cy-2+q)))[1];
//							R_value+=(frame.at<cv::Vec3b>(cv::Point(cx-2+p,cy-2+q)))[2];
//					      }
//					B_value=B_value/25;
//					G_value=G_value/25;
//					R_value=R_value/25;
					 B_value=(frame.at<cv::Vec3b>(cv::Point(cx,cy)))[0];
					 G_value=(frame.at<cv::Vec3b>(cv::Point(cx,cy)))[1];
					 R_value=(frame.at<cv::Vec3b>(cv::Point(cx,cy)))[2];
					//ss<<"B:"<<B_value<<" G:"<<G_value<<" R:"<<R_value<<",,,";
          std::string str_color="red";
					if(R_value>220&&G_value>220)
						str_color="yellow";
	         if(G_value>R_value&&G_value>B_value)
	              str_color="green";
	        ss<<str_color;
	        //for debugging ss<<str_color<<",";
					//for debugging ss<<cx<<","<<cy<<";";
          au_core::Roi roi;
          roi.xOffset = xoffset;
          roi.yOffset = yoffset;
          roi.width = t_width;
          roi.height = t_height;
          roi.type = ss.str();
					roi.color=str_color;
          roiArray.push_back(roi);
					//for debugging msg.data = ss.str();
					//publish the message.
					//for debugging pub.publish(msg);
					//send a message to rosout with the details.
					//for debugging ROS_INFO("%s", msg.data.c_str());
					//wait until time for another iteration.
					//for debugging rosrate.sleep();
				}// end of if
			  }// end of for
			  }//end of if( ell_out != NULL)
		 else
			 isEffective=false;
			  //return 0;
		 //for debugging cv::imshow(source_window, ViewControl );
		 //outputVideo << ViewControl;
		//for debugging if(cv::waitKey(delay)>=0)
			//for debugging stop=true;
     return roiArray;
}

PImageDouble read_pgm_image_double1(cv::Mat cv_image )
{
	PImageDouble image;
	/* get memory */
	int xsize=cv_image.size().width;
	int ysize=cv_image.size().height;
	  image = new_PImageDouble( xsize, ysize );

	  /* read data */

	  for(int y=0; y<ysize; y++)
	    for(int x=0; x<xsize; x++)
	    {
	    	cv::Scalar intensity=cv_image.at<uchar>(y, x);
	      image->data[ x + y * xsize ] = intensity.val[0];
	    }

	  return image;
}

 std::string doubleToString(double db)
 {
	 std::ostringstream doubleStreams;
	 		     	doubleStreams<<db;
	 	return doubleStreams.str();
 }

} // end of namespace
