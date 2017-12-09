#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <sl/Camera.hpp>
#include <sl/Core.hpp>
#include <sl/defines.hpp>

using namespace sl;
using namespace std;

cv::Mat slMat2cvMat(sl::Mat& input);

int main(int argc, char **argv)
{
    Camera zed;
    InitParameters initParameters;
    initParameters.camera_fps = 30;
    initParameters.depth_mode = sl::DEPTH_MODE_NONE;

    //open the ZED
    ERROR_CODE err = zed.open(initParameters);
    if(err != SUCCESS)
    {
        cout << errorCode2str(err) << endl;
        zed.close();
        return 1;
    }

    //set runtime parameters after opening the camera
    RuntimeParameters runtime_parameters;
    runtime_parameters.sensing_mode = SENSING_MODE_STANDARD;

    Resolution image_size = zed.getResolution();
    int width = image_size.width;
    int height = image_size.height;

    cv::Mat left_image_c4(height, width, CV_8UC4);
    cv::Mat right_image_c4(height, width, CV_8UC4);
    cv::Mat left_image(height, width, CV_8UC3);
    cv::Mat right_image(height, width, CV_8UC3);

    vector<uchar> compressed_left_stream;
    vector<uchar> compressed_right_stream;

    //setup key, images, times
    char key = ' ';
    sl::Mat sl_mat_left(image_size, sl::MAT_TYPE_8U_C4);
    sl::Mat sl_mat_right(image_size, sl::MAT_TYPE_8U_C4);
    cout << "Press 'q' to exit..." << endl;
    int picture_ID_l = 0;
    int picture_ID_r = 0;

    cv::namedWindow("left", CV_WINDOW_NORMAL);
    cv::namedWindow("right", CV_WINDOW_NORMAL);

    while(1)
    {
        if(zed.grab(runtime_parameters) == SUCCESS)
        {
            zed.retrieveImage(sl_mat_left, sl::VIEW_LEFT);
            zed.retrieveImage(sl_mat_right, sl::VIEW_RIGHT);

            //slmat-->cvmat
            left_image_c4 = slMat2cvMat(sl_mat_left);
            right_image_c4 = slMat2cvMat(sl_mat_right);

            //mat_c4-->mat_c3
            cvtColor(left_image_c4, left_image, CV_BGRA2BGR);
            cvtColor(right_image_c4, right_image, CV_BGRA2BGR);

            cv::imshow("left", left_image);
            cv::imshow("right", right_image);

            key = cv::waitKey(40);
            cout << "save image: " << endl;

            while(key != '27')
            {
                const int bufSize = 256;
                char Base_name_l[bufSize], Base_name_r[bufSize];

                std::string file_path_l = "/home/chenxinghui/save_ZED_pictures/savepictures/pictures/image_left/";
                sprintf(Base_name_l, "%06d.png", picture_ID_l);
                std::string image_path_l = file_path_l + Base_name_l;
                cv::imwrite(image_path_l, left_image);
                ++picture_ID_l;

                std::string file_path_r = "/home/chenxinghui/save_ZED_pictures/savepictures/pictures/image_right/";
                sprintf(Base_name_r, "%06d.png", picture_ID_r);
                std::string image_path_r = file_path_r + Base_name_r;
                cv::imwrite(image_path_r, right_image);
                ++picture_ID_r;
                break;
            }
        }
    }
    zed.close();
    return 0;
}

cv::Mat slMat2cvMat(sl::Mat& input)
{
    int cv_type = -1;
    switch(input.getDataType())
    {
    case sl::MAT_TYPE_32F_C1:
        cv_type = CV_32FC1;
        break;
    case sl::MAT_TYPE_32F_C2:
        cv_type = CV_32FC2;
        break;
    case sl::MAT_TYPE_32F_C3:
        cv_type = CV_32FC3;
        break;
    case sl::MAT_TYPE_32F_C4:
        cv_type = CV_32FC4;
        break;
    case sl::MAT_TYPE_8U_C1:
        cv_type = CV_8UC1;
        break;
    case sl::MAT_TYPE_8U_C2:
        cv_type = CV_8UC2;
        break;
    case sl::MAT_TYPE_8U_C3:
        cv_type = CV_8UC3;
        break;
    case sl::MAT_TYPE_8U_C4:
        cv_type = CV_8UC4;
        break;
    }
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_CPU));
}












