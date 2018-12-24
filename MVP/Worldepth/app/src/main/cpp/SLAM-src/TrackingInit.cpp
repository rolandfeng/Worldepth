//
// Created by Michael Duan on 12/1/18.
//

#include "TrackingInit.h"

namespace SLAM
{
    TrackingInit::TrackingInit(string &strVocFile, string &strSettingsFile): mReset(false), mActivateLocalizationMode(false), mDeactivateLocalizationMode(false) {

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
        mtTracking = new thread(&TrackingInit::beginTracking, this);
        //mtLoopClosing = new thread()

        //mtLocalMapping = new thread(&LocalMapping::Run, mpLocalMapper);
    }

    void TrackingInit::sendToFrameList(Frame *frame) {
        frameList->addFrame(frame);
    }

    void TrackingInit::sendToKeyFrameList(SLAM::KeyFrame *keyFrame) {
        frameList->addKeyFrame(keyFrame);
    }

    /* void TrackingInit::processing() {
        while (TrackingInit::isProcessing) {
            TrackingInit::sendToProcess();
        }
    }

    void TrackingInit::sendToProcess() {

        frameList->getFrameDatabase()[0];

        //call function on FrameList[0]
    } */
    void TrackingInit::Reset() {
        mReset = true;
    }

    void TrackingInit::Stop() {
        //mtTracking->RequestFinish();
    }

    cv::Mat TrackingInit::beginTracking(const cv::Mat im, const double timestamp) {
        cv::Mat Tcw = mTracker->GrabImageMonocular(im, timestamp);

        mTrackingState = mTracker->mState;
        mTrackedMapPoints = mTracker->mCurrentFrame.mvpMapPoints;
        mTrackedKeyPointsUn = mTracker->mCurrentFrame.mvKeysUn;

        return Tcw;

    }
}
