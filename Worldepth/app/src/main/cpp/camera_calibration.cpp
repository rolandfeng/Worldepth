#include <camera_calibration.h>

using namespace cv;
using namespace std;
namespace calib {

    string calib::Settings::internalPath = "";

    static void help() {
        cout << "This is a camera calibration sample." << endl
             << "Usage: camera_calibration [configuration_file -- default ./default.xml]" << endl
             << "Near the sample file you'll find the configuration file, which has detailed help of "
                "how to edit it.  It may be any OpenCV supported file format XML/YAML." << endl;
    }


    Settings::Settings(std::string intPath) {
        help();
        calib::Settings::internalPath = intPath;
        //! [file_read]
        //const string inputSettingsFile = argc > 1 ? argv[1] : "default.xml";
        string inputSettingsFile = internalPath + "/calib_data.xml";
        FileStorage fs(inputSettingsFile, FileStorage::READ); // Read the settings
        if (!fs.isOpened()) {
            cout << "Could not open the configuration file: \"" << inputSettingsFile << "\""
                 << endl;
            return;
        }
        read(fs["Settings"]);
        fs.release();                                         // close Settings file
        //! [file_read]

        //FileStorage fout("settings.yml", FileStorage::WRITE); // write config as YAML
        //fout << "Settings" << s;

        if (!goodInput) {
            cout << "Invalid input detected. Application stopping. " << endl;
            return;
        }

        mode = CAPTURING;
        const Scalar RED(0, 0, 255), GREEN(0, 255, 0);
        prevTimestamp = 0;
    }


    void Settings::write(
            FileStorage &fs) const                        //Write serialization for this class
    {
        fs << "{"
           << "BoardSize_Width" << boardSize.width
           << "BoardSize_Height" << boardSize.height
           << "Square_Size" << squareSize
           << "Calibrate_Pattern" << patternToUse
           << "Calibrate_NrOfFrameToUse" << nrFrames
           << "Calibrate_FixAspectRatio" << aspectRatio
           << "Calibrate_AssumeZeroTangentialDistortion" << calibZeroTangentDist
           << "Calibrate_FixPrincipalPointAtTheCenter" << calibFixPrincipalPoint

           << "Write_DetectedFeaturePoints" << writePoints
           << "Write_extrinsicParameters" << writeExtrinsics
           << "Write_outputFileName" << outputFileName

           << "Show_UndistortedImage" << showUndistorsed

           << "Input_FlipAroundHorizontalAxis" << flipVertical
           << "Input_Delay" << delay
           << "Input" << input
           << "}";
    }

    void Settings::read(
            const FileNode &node)                          //Read serialization for this class
    {
        node["BoardSize_Width"] >> boardSize.width;
        node["BoardSize_Height"] >> boardSize.height;
        node["Calibrate_Pattern"] >> patternToUse;
        node["Square_Size"] >> squareSize;
        node["Calibrate_NrOfFrameToUse"] >> nrFrames;
        node["Calibrate_FixAspectRatio"] >> aspectRatio;
        node["Write_DetectedFeaturePoints"] >> writePoints;
        node["Write_extrinsicParameters"] >> writeExtrinsics;
        node["Write_outputFileName"] >> outputFileName;
        node["Calibrate_AssumeZeroTangentialDistortion"] >> calibZeroTangentDist;
        node["Calibrate_FixPrincipalPointAtTheCenter"] >> calibFixPrincipalPoint;
        node["Calibrate_UseFisheyeModel"] >> useFisheye;
        node["Input_FlipAroundHorizontalAxis"] >> flipVertical;
        node["Show_UndistortedImage"] >> showUndistorsed;
        node["Input"] >> input;
        node["Input_Delay"] >> delay;
        node["Fix_K1"] >> fixK1;
        node["Fix_K2"] >> fixK2;
        node["Fix_K3"] >> fixK3;
        node["Fix_K4"] >> fixK4;
        node["Fix_K5"] >> fixK5;

        validate();
    }

    void Settings::validate() {
        goodInput = true;
        if (boardSize.width <= 0 || boardSize.height <= 0) {
            cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << endl;
            goodInput = false;
        }
        if (squareSize <= 10e-6) {
            cerr << "Invalid square size " << squareSize << endl;
            goodInput = false;
        }
        if (nrFrames <= 0) {
            cerr << "Invalid number of frames " << nrFrames << endl;
            goodInput = false;
        }

        if (input.empty())      // Check for valid input
            inputType = INVALID;
        else {
            if (input[0] >= '0' && input[0] <= '9') {
                stringstream ss(input);
                ss >> cameraID;
                inputType = CAMERA;
            } else {
                if (isListOfImages(input) && readStringList(input, imageList)) {
                    inputType = IMAGE_LIST;
                    nrFrames = (nrFrames < (int) imageList.size()) ? nrFrames
                                                                   : (int) imageList.size();
                } else
                    inputType = VIDEO_FILE;
            }
            if (inputType == CAMERA)
                //inputCapture.open(cameraID);
                if (inputType == VIDEO_FILE)
                    inputCapture.open(input);
        }
        if (inputType == INVALID) {
            cerr << " Input does not exist: " << input;
            goodInput = false;
        }

        flag = 0;
        if (calibFixPrincipalPoint) flag |= CALIB_FIX_PRINCIPAL_POINT;
        if (calibZeroTangentDist) flag |= CALIB_ZERO_TANGENT_DIST;
        if (aspectRatio) flag |= CALIB_FIX_ASPECT_RATIO;
        if (fixK1) flag |= CALIB_FIX_K1;
        if (fixK2) flag |= CALIB_FIX_K2;
        if (fixK3) flag |= CALIB_FIX_K3;
        if (fixK4) flag |= CALIB_FIX_K4;
        if (fixK5) flag |= CALIB_FIX_K5;

        if (useFisheye) {
            // the fisheye model has its own enum, so overwrite the flags
            flag = fisheye::CALIB_FIX_SKEW | fisheye::CALIB_RECOMPUTE_EXTRINSIC;
            if (fixK1) flag |= fisheye::CALIB_FIX_K1;
            if (fixK2) flag |= fisheye::CALIB_FIX_K2;
            if (fixK3) flag |= fisheye::CALIB_FIX_K3;
            if (fixK4) flag |= fisheye::CALIB_FIX_K4;
            if (calibFixPrincipalPoint) flag |= fisheye::CALIB_FIX_PRINCIPAL_POINT;
        }

        calibrationPattern = NOT_EXISTING;
        if (!patternToUse.compare("CHESSBOARD")) calibrationPattern = CHESSBOARD;
        if (!patternToUse.compare("CIRCLES_GRID")) calibrationPattern = CIRCLES_GRID;
        if (!patternToUse.compare("ASYMMETRIC_CIRCLES_GRID"))
            calibrationPattern = ASYMMETRIC_CIRCLES_GRID;
        if (calibrationPattern == NOT_EXISTING) {
            cerr << " Camera calibration mode does not exist: " << patternToUse << endl;
            goodInput = false;
        }
        atImageList = 0;

    }


    bool Settings::readStringList(const string &filename, vector<string> &l) {
        l.clear();
        FileStorage fs(filename, FileStorage::READ);
        if (!fs.isOpened())
            return false;
        FileNode n = fs.getFirstTopLevelNode();
        if (n.type() != FileNode::SEQ)
            return false;
        FileNodeIterator it = n.begin(), it_end = n.end();
        for (; it != it_end; ++it)
            l.push_back((string) *it);
        return true;
    }

    bool Settings::isListOfImages(const string &filename) {
        string s(filename);
        // Look for file extension
        if (s.find(".xml") == string::npos && s.find(".yaml") == string::npos &&
            s.find(".yml") == string::npos)
            return false;
        else
            return true;
    }


    //! [get_input]
    void Settings::processImage(Mat &view) {
        //Mat view;
        bool blinkOutput = false;

        if (mode == CALIBRATED)
            return;
        //view = s.nextImage();

        //-----  If no more image, or got enough, then stop calibration and show result -------------
        if (mode == CAPTURING && imagePoints.size() >= (size_t) nrFrames) {
            if (runCalibrationAndSave(imageSize, cameraMatrix, distCoeffs, imagePoints)) {
                mode = CALIBRATED;
                return;
            } else {
                mode = DETECTION;
            }
        }
        if (view.empty())          // If there are no more images stop the loop
        {
            // if calibration threshold was not reached yet, calibrate now
            if (mode != CALIBRATED && !imagePoints.empty()) {
                runCalibrationAndSave(imageSize, cameraMatrix, distCoeffs, imagePoints);
                mode = CALIBRATED;
            }
            return;
        }
        //! [get_input]

        imageSize = view.size();  // Format input image.
        if (flipVertical) flip(view, view, 0);

        //! [find_pattern]
        vector<Point2f> pointBuf;

        bool found;

        int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;

        if (!useFisheye) {
            // fast check erroneously fails with high distortions like fisheye
            chessBoardFlags |= CALIB_CB_FAST_CHECK;
        }

        switch (calibrationPattern) // Find feature points on the input format
        {
            case Settings::CHESSBOARD:
                found = findChessboardCorners(view, boardSize, pointBuf, chessBoardFlags);
                break;
            case Settings::CIRCLES_GRID:
                found = findCirclesGrid(view, boardSize, pointBuf);
                break;
            case Settings::ASYMMETRIC_CIRCLES_GRID:
                found = findCirclesGrid(view, boardSize, pointBuf, CALIB_CB_ASYMMETRIC_GRID);
                break;
            default:
                found = false;
                break;
        }
        //! [find_pattern]
        //! [pattern_found]
        if (found)                // If done with success,
        {
            // improve the found corners' coordinate accuracy for chessboard
            if (calibrationPattern == Settings::CHESSBOARD) {
                Mat viewGray;
                cvtColor(view, viewGray, COLOR_BGR2GRAY);
                cornerSubPix(viewGray, pointBuf, Size(11, 11),
                             Size(-1, -1),
                             TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
            }

            if (mode == CAPTURING &&  // For camera only take new samples after delay time
                (clock() - prevTimestamp > delay * 1e-3 * CLOCKS_PER_SEC)) {
                imagePoints.push_back(pointBuf);
                prevTimestamp = clock();
                blinkOutput = inputCapture.isOpened();
            }

            // Draw the corners.
            //drawChessboardCorners(view, s.boardSize, Mat(pointBuf), found);
        }
        //! [pattern_found]
        //----------------------------- Output Text ------------------------------------------------
        //! [output_text]
        string msg = (mode == CAPTURING) ? "100/100" :
                     mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
        int baseLine = 0;
        Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
        Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

        if (mode == CAPTURING) {
            if (showUndistorsed)
                msg = format("%d/%d Undist", (int) imagePoints.size(), nrFrames);
            else
                msg = format("%d/%d", (int) imagePoints.size(), nrFrames);
        }

        //putText(view, msg, textOrigin, 1, 1, mode == CALIBRATED ? GREEN : RED);

        if (blinkOutput)
            bitwise_not(view, view);
        //! [output_text]
        //------------------------- Video capture  output  undistorted ------------------------------
        //! [output_undistorted]
        if (mode == CALIBRATED && showUndistorsed) {
            Mat temp = view.clone();
            if (useFisheye)
                cv::fisheye::undistortImage(temp, view, cameraMatrix, distCoeffs);
            else
                undistort(temp, view, cameraMatrix, distCoeffs);
        }
        //! [output_undistorted]
        /*//------------------------------ Show image and check for input commands -------------------
        //! [await_input]
        imshow("Image View", view);
        char key = (char) waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

        if (key == ESC_KEY)
            break;

        if (key == 'u' && mode == CALIBRATED)
            s.showUndistorsed = !s.showUndistorsed;

        if (s.inputCapture.isOpened() && key == 'g') {
            mode = CAPTURING;
            imagePoints.clear();
        }*/
        //! [await_input]

    }


//! [compute_errors]
    static double computeReprojectionErrors(const vector<vector<Point3f> > &objectPoints,
                                            const vector<vector<Point2f> > &imagePoints,
                                            const vector<Mat> &rvecs, const vector<Mat> &tvecs,
                                            const Mat &cameraMatrix, const Mat &distCoeffs,
                                            vector<float> &perViewErrors, bool fisheye) {
        vector<Point2f> imagePoints2;
        size_t totalPoints = 0;
        double totalErr = 0, err;
        perViewErrors.resize(objectPoints.size());

        for (size_t i = 0; i < objectPoints.size(); ++i) {
            if (fisheye) {
                fisheye::projectPoints(objectPoints[i], imagePoints2, rvecs[i], tvecs[i],
                                       cameraMatrix,
                                       distCoeffs);
            } else {
                projectPoints(objectPoints[i], rvecs[i], tvecs[i], cameraMatrix, distCoeffs,
                              imagePoints2);
            }
            err = norm(imagePoints[i], imagePoints2, NORM_L2);

            size_t n = objectPoints[i].size();
            perViewErrors[i] = (float) std::sqrt(err * err / n);
            totalErr += err * err;
            totalPoints += n;
        }

        return std::sqrt(totalErr / totalPoints);
    }

//! [compute_errors]
//! [board_corners]
    static void calcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f> &corners,
                                         Settings::Pattern patternType /*= Settings::CHESSBOARD*/) {
        corners.clear();

        switch (patternType) {
            case Settings::CHESSBOARD:
            case Settings::CIRCLES_GRID:
                for (int i = 0; i < boardSize.height; ++i)
                    for (int j = 0; j < boardSize.width; ++j)
                        corners.push_back(Point3f(j * squareSize, i * squareSize, 0));
                break;

            case Settings::ASYMMETRIC_CIRCLES_GRID:
                for (int i = 0; i < boardSize.height; i++)
                    for (int j = 0; j < boardSize.width; j++)
                        corners.push_back(Point3f((2 * j + i % 2) * squareSize, i * squareSize, 0));
                break;
            default:
                break;
        }
    }

    //! [board_corners]
    static bool runCalibration(Settings &s, Size &imageSize, Mat &cameraMatrix, Mat &distCoeffs,
                               vector<vector<Point2f> >
                               imagePoints, vector<Mat> &rvecs, vector<Mat> &tvecs,
                               vector<float> &reprojErrs,
                               double &totalAvgErr) {
        //! [fixed_aspect]
        cameraMatrix = Mat::eye(3, 3, CV_64F);
        if (s.flag & CALIB_FIX_ASPECT_RATIO) {
            s.aspectRatio = (float) imageSize.width / imageSize.height;
            cameraMatrix.at<double>(0, 0) = s.aspectRatio;
            cameraMatrix.at<double>(1, 1) = 1.;
            cameraMatrix.at<double>(2, 2) = 1.;
        }
        //! [fixed_aspect]
        if (s.useFisheye) {
            distCoeffs = Mat::zeros(4, 1, CV_64F);
        } else {
            distCoeffs = Mat::zeros(8, 1, CV_64F);
        }
        vector<vector<Point3f> > objectPoints(1);
        calcBoardCornerPositions(s.boardSize, s.squareSize, objectPoints[0], s.calibrationPattern);
        objectPoints.resize(imagePoints.size(), objectPoints[0]);

        //Find intrinsic and extrinsic camera parameters
        double rms;

        if (s.useFisheye) {
            Mat _rvecs, _tvecs;
            rms = fisheye::calibrate(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs,
                                     _rvecs,
                                     _tvecs, s.flag);

            rvecs.reserve(_rvecs.rows);
            tvecs.reserve(_tvecs.rows);
            for (int i = 0; i < int(objectPoints.size()); i++) {
                rvecs.push_back(_rvecs.row(i));
                tvecs.push_back(_tvecs.row(i));
            }
        } else {
            double temp1 = cameraMatrix.at<double>(0, 0);
            double temp2 = cameraMatrix.at<double>(1, 1);
            rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs,
                                  rvecs, tvecs,
                                  s.flag);
        }

        cout << "Re-projection error reported by calibrateCamera: " << rms <<
             endl;

        bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

        totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints, rvecs, tvecs,
                                                cameraMatrix,
                                                distCoeffs, reprojErrs, s.useFisheye);

        return
                ok;
    }

// Print camera parameters to the output file
void Settings::saveCameraParams(Settings &s, Size &imageSize, Mat &cameraMatrix, Mat &distCoeffs,
                             const vector<Mat> &rvecs, const vector<Mat> &tvecs,
                             const vector<float> &reprojErrs,
                             const vector<vector<Point2f> > &imagePoints,
                             double totalAvgErr) {
    try {
        FileStorage fs(internalPath + s.outputFileName, FileStorage::WRITE /*| FileStorage::FORMAT_YAML*/);


    time_t tm;
    time(&tm);
    struct tm *t2 = localtime(&tm);
    char buf[1024];
    strftime(buf, sizeof(buf), "%c", t2);

    fs << "calibration_time" << buf;
/*
    if (!rvecs.empty() || !reprojErrs.empty())
        fs << "nr_of_frames" << (int) std::max(rvecs.size(), reprojErrs.size());
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "board_width" << s.boardSize.width;
    fs << "board_height" << s.boardSize.height;
    fs << "square_size" << s.squareSize;

    if (s.flag & CALIB_FIX_ASPECT_RATIO)
        fs << "fix_aspect_ratio" << s.aspectRatio;

    if (s.flag) {
        std::stringstream flagsStringStream;
        if (s.useFisheye) {
            flagsStringStream << "flags:"
                              << (s.flag & fisheye::CALIB_FIX_SKEW ? " +fix_skew" : "")
                              << (s.flag & fisheye::CALIB_FIX_K1 ? " +fix_k1" : "")
                              << (s.flag & fisheye::CALIB_FIX_K2 ? " +fix_k2" : "")
                              << (s.flag & fisheye::CALIB_FIX_K3 ? " +fix_k3" : "")
                              << (s.flag & fisheye::CALIB_FIX_K4 ? " +fix_k4" : "")
                              << (s.flag & fisheye::CALIB_RECOMPUTE_EXTRINSIC
                                  ? " +recompute_extrinsic" : "");
        } else {
            flagsStringStream << "flags:"
                              << (s.flag & CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "")
                              << (s.flag & CALIB_FIX_ASPECT_RATIO ? " +fix_aspectRatio" : "")
                              << (s.flag & CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "")
                              << (s.flag & CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "")
                              << (s.flag & CALIB_FIX_K1 ? " +fix_k1" : "")
                              << (s.flag & CALIB_FIX_K2 ? " +fix_k2" : "")
                              << (s.flag & CALIB_FIX_K3 ? " +fix_k3" : "")
                              << (s.flag & CALIB_FIX_K4 ? " +fix_k4" : "")
                              << (s.flag & CALIB_FIX_K5 ? " +fix_k5" : "");
        }
        fs.writeComment(flagsStringStream.str());
    }

    fs << "flags" << s.flag;

    fs << "fisheye_model" << s.useFisheye;*/

            fs << "camera_matrix" << cameraMatrix;
            /*fs << "Camera_fx" << (double) cameraMatrix.at<float>(0, 0);
            fs.write("Camera_fy", (double) cameraMatrix.at<float>(1, 1));
            fs.write("Camera_cx", (double) cameraMatrix.at<float>(0, 2));
            fs.write("Camera_cy", (double) cameraMatrix.at<float>(1, 2));*/

            fs << "distortion_coefficients" << distCoeffs;
            /*fs.write("Camera_k1", (double) distCoeffs.at<float>(0));
            fs.write("Camera_k2", (double) distCoeffs.at<float>(1));
            fs.write("Camera_p1", (double) distCoeffs.at<float>(2));
            fs.write("Camera_p2", (double) distCoeffs.at<float>(3));
            fs.write("Camera_k3", (double) distCoeffs.at<float>(4));*/

            fs.write("Camera_fps", 15);
            fs.write("Camera_RGB", 1);
            fs.write("ORBextractor_nFeatures", 2000);
            fs.write("ORBextractor_scaleFactor", 1.2);
            fs.write("ORBextractor_nLevels", 8);
            fs.write("ORBextractor_iniThFAST", 20);
            fs.write("ORBextractor_minThFAST", 7);

/*    fs << "avg_reprojection_error" << totalAvgErr;
    if (s.writeExtrinsics && !reprojErrs.empty())
        fs << "per_view_reprojection_errors" << Mat(reprojErrs);

    if (s.writeExtrinsics && !rvecs.empty() && !tvecs.empty()) {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int) rvecs.size(), 6, CV_MAKETYPE(rvecs[0].type(), 1));
        bool needReshapeR = rvecs[0].depth() != 1 ? true : false;
        bool needReshapeT = tvecs[0].depth() != 1 ? true : false;

        for (size_t i = 0; i < rvecs.size(); i++) {
            Mat r = bigmat(Range(int(i), int(i + 1)), Range(0, 3));
            Mat t = bigmat(Range(int(i), int(i + 1)), Range(3, 6));

            if (needReshapeR)
                rvecs[i].reshape(1, 1).copyTo(r);
            else {
                //.t() is MatExpr (not Mat) so we can use assignment operator
                CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
                r = rvecs[i].t();
            }

            if (needReshapeT)
                tvecs[i].reshape(1, 1).copyTo(t);
            else {
                CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
                t = tvecs[i].t();
            }
        }
        fs.writeComment("a set of 6-tuples (rotation vector + translation vector) for each view");
        fs << "extrinsic_parameters" << bigmat;
    }

    if (s.writePoints && !imagePoints.empty()) {
        Mat imagePtMat((int) imagePoints.size(), (int) imagePoints[0].size(), CV_32FC2);
        for (size_t i = 0; i < imagePoints.size(); i++) {
            Mat r = imagePtMat.row(int(i)).reshape(2, imagePtMat.cols);
            Mat imgpti(imagePoints[i]);
            imgpti.copyTo(r);
        }
        fs << "image_points" << imagePtMat;
    }*/
            fs.release();
        } catch (Exception e) {
            int error = 1;
        }
    }

//! [run_and_save]
    bool Settings::runCalibrationAndSave(Size imageSize, Mat &cameraMatrix, Mat &distCoeffs,
                                         vector<vector<Point2f> > imagePoints) {
        vector<Mat> rvecs, tvecs;
        vector<float> reprojErrs;
        double totalAvgErr = 0;

        bool ok = runCalibration(*this, imageSize, cameraMatrix, distCoeffs, imagePoints, rvecs,
                                 tvecs,
                                 reprojErrs,
                                 totalAvgErr);
        cout << (ok ? "Calibration succeeded" : "Calibration failed")
             << ". avg re projection error = " << totalAvgErr << endl;

        if (ok)
            saveCameraParams(*this, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, reprojErrs,
                             imagePoints,
                             totalAvgErr);
        return ok;
    }

//! [run_and_save]
}
