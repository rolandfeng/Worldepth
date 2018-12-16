//
// Created by Michael Duan on 12/1/18.
//

#include "TrackingInit.h"

namespace SLAM
{
    TrackingInit::TrackingInit(const string &strVocFile, const string &strSettingsFile) {
        isProcessing = true;

        frameList = new FrameList();

        map = new Map();

        mVocabulary = new ORBVocabulary();
        bool bVocLoad = mVocabulary->loadFromTextFile(strVocFile);
        if(!bVocLoad)
        {
            cerr << "Wrong path to vocabulary. " << endl;
            cerr << "Failed to open at: " << strVocFile << endl;
            exit(-1);
        }

        mTracker = new Tracking(this, mVocabulary, map, strSettingsFile);

        mptTracking = new thread(&TrackingInit::processing, this);
        //mptLoopClosing = new thread()

        //mptLocalMapping = new thread(&LocalMapping::Run, mpLocalMapper);
    }

    void TrackingInit::sendToFrameList(Frame *frame) {
        frameList->addFrame(frame);
    }

    void TrackingInit::sendToKeyFrameList(SLAM::KeyFrame *keyFrame) {
        frameList->addKeyFrame(keyFrame);
    }

    void TrackingInit::processing() {
        /* while (TrackingInit::isProcessing) {
            TrackingInit::sendToProcess();
        } */
    }

    void TrackingInit::sendToProcess() {

        frameList->getFrameDatabase()[0];

        //call function on FrameList[0]
    }

    cv::Mat beginTracking(const cv::Mat &im, const double &timeStamp) {
        //cv::Mat Tcw = mTracker->GrabImageMonocular(im, timestamp);

        /* mTrackingState = mpTracker->mState;
        mTrackedMapPoints = mpTracker->mCurrentFrame.mvpMapPoints;
        mTrackedKeyPointsUn = mpTracker->mCurrentFrame.mvKeysUn;

        return Tcw; */

    }
}
