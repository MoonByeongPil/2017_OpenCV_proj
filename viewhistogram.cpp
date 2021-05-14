#include <cv.h>
#include <highgui.h>
#include <math.h>

void histogram(char* w_name, IplImage* img)
{
	IplImage* canvas;
	int       hist[256];
	double    scale = 1;
	int       i, j, max = 0;


	
	//////////////////////////////////////////////////////////////////////////
	// histogram을 표현할 이미지 canvas 생성

	canvas = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 1);
	cvSet(canvas, CV_RGB(255, 255, 255), 0);


	//////////////////////////////////////////////////////////////////////////
	// hist 초기화 및 각 밝기의 빈도수 계산

	for (i = 0; i < 256; i++)
	{
		hist[i] = 0;
	}
	
	for (i = 0; i < img->height; i++)
	{
		for (j = 0; j < img->width; j++)
		{
			hist[(uchar)img->imageData[(i * img->widthStep) + j]]++;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// max : 가장 많이 나온 밝기의 개수
	// 동일한 scale로 여러 이미지의 histogram을 보고싶을 경우
	// max 값을 임의로 고정시키면 된다
	// 여기서는 최대로 나온 개수를 256*60개라고 임의로 지정

	max = 256 * 50;		


	//////////////////////////////////////////////////////////////////////////
	// 화면을 꽉 채워서 보고싶으면 아래의 for문을 이용해 hist 중 최대 값을 찾는다

	/*
	for (i = 0; i < 256; i++)
	{
		max = hist[i] > max ? hist[i] : max;
	}
	*/
	

	//////////////////////////////////////////////////////////////////////////
	// canvas 창에 맞도록 scale 값 설정

	scale = max > canvas->height ? (double)canvas->height/max : 1.;


	//////////////////////////////////////////////////////////////////////////
	// canvas에 세로 줄을 그림
	// 줄을 canvas의 밑에서부터 그려야하기 때문에 맨 밑의 포인트(pt2)부터 그림

	for (i = 0; i < 256; i++)
	{
		CvPoint pt1 = cvPoint(i, canvas->height - (hist[i] * scale));
		CvPoint pt2 = cvPoint(i, canvas->height);
		cvLine(canvas, pt1, pt2, CV_RGB(0,0,0), 1, 8, 0);
	}


	//////////////////////////////////////////////////////////////////////////
	// w_name이라는 창에 canvas 이미지를 출력
	
	cvShowImage(w_name, canvas);


	cvReleaseImage(&canvas);
}


int main(int argc, char* argv[])
{	
	IplImage* img1 =cvLoadImage("p-3_tamplet.jpg",-1);

	histogram("hist1", img1);

	cvWaitKey(0);
	
	cvDestroyAllWindows();
	cvReleaseImage(&img1);


	return 0;
}
