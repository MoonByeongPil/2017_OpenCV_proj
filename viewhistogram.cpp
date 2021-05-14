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
	// histogram�� ǥ���� �̹��� canvas ����

	canvas = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 1);
	cvSet(canvas, CV_RGB(255, 255, 255), 0);


	//////////////////////////////////////////////////////////////////////////
	// hist �ʱ�ȭ �� �� ����� �󵵼� ���

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
	// max : ���� ���� ���� ����� ����
	// ������ scale�� ���� �̹����� histogram�� ������� ���
	// max ���� ���Ƿ� ������Ű�� �ȴ�
	// ���⼭�� �ִ�� ���� ������ 256*60����� ���Ƿ� ����

	max = 256 * 50;		


	//////////////////////////////////////////////////////////////////////////
	// ȭ���� �� ä���� ��������� �Ʒ��� for���� �̿��� hist �� �ִ� ���� ã�´�

	/*
	for (i = 0; i < 256; i++)
	{
		max = hist[i] > max ? hist[i] : max;
	}
	*/
	

	//////////////////////////////////////////////////////////////////////////
	// canvas â�� �µ��� scale �� ����

	scale = max > canvas->height ? (double)canvas->height/max : 1.;


	//////////////////////////////////////////////////////////////////////////
	// canvas�� ���� ���� �׸�
	// ���� canvas�� �ؿ������� �׷����ϱ� ������ �� ���� ����Ʈ(pt2)���� �׸�

	for (i = 0; i < 256; i++)
	{
		CvPoint pt1 = cvPoint(i, canvas->height - (hist[i] * scale));
		CvPoint pt2 = cvPoint(i, canvas->height);
		cvLine(canvas, pt1, pt2, CV_RGB(0,0,0), 1, 8, 0);
	}


	//////////////////////////////////////////////////////////////////////////
	// w_name�̶�� â�� canvas �̹����� ���
	
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
