#include "COcvTask.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

cv::String face_cascade_name = "haarcascade_frontalface_alt.xml";
cv::String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
cv::CascadeClassifier face_cascade;
cv::CascadeClassifier eyes_cascade;
cv::String window_name = "Capture - Face detection";

COcvTask::COcvTask()
:_width(0), _height(0)
{

}

int COcvTask::svc(void)
{
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc start\n"));
	if (!face_cascade.load(face_cascade_name)) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "fase cascade"), -1);
	if (!eyes_cascade.load(eyes_cascade_name)) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "eyes cascade"), -1);

	ACE_Message_Block *message;
	for (;;){
		if (this->getq(message) == -1) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "getq"), -1);

		if (message->msg_type() == ACE_Message_Block::MB_HANGUP) {
			message->release();
			ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc MB_HANGUP received\n"));
			break;
		}

		ACE_ASSERT(_width > 0 && _height > 0);
		cv::Mat frame(cv::Size(_width,_height), CV_8UC1, message->rd_ptr(), cv::Mat::AUTO_STEP);

		detectAndDisplay(frame);
		cv::waitKey(1);

		ACE_DEBUG((LM_DEBUG, "."));
		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc end\n"));
	return 0;
}

void COcvTask::detectAndDisplay(cv::Mat frame)
{
	std::vector<cv::Rect> faces;
	cv::Mat frame_gray = frame;

	//cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	cv::equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(255, 0, 255), 2, 8, 0);
		ACE_DEBUG((LM_DEBUG, "faces[%d],width(%d),height(%d) ",i,faces[i].width,faces[i].height));

		cv::Mat faceROI = frame_gray(faces[i]);
		std::vector<cv::Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

		for (size_t j = 0; j < eyes.size(); j++)
		{
			cv::Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, eye_center, radius, cv::Scalar(255, 0, 0), 3, 8, 0);
			ACE_DEBUG((LM_DEBUG, "eyes[%d],width(%d),height(%d) ", j, eyes[j].width, eyes[j].height));
		}
		ACE_DEBUG((LM_DEBUG, "\n"));
	}
	//-- Show what you got
	cv::imshow(window_name, frame);
}