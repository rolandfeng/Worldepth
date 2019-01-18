package com.example.leodw.worldepth.ui.Reconstruction;

import android.arch.lifecycle.ViewModelProviders;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.leodw.worldepth.R;
import com.example.leodw.worldepth.slam.ReconVM;

import androidx.navigation.Navigation;

public class ReconstructionFragment extends Fragment {

    private static final String TAG = "ReconstructionFragment";

    private ReconVM mReconVM;
    private TextView mSlamProgress;

    private Button mNextButton;
    private Button mBackButton;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.reconstruction_fragment, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        mReconVM = ViewModelProviders.of(getActivity()).get(ReconVM.class);
        mReconVM.getReconProgress().observe(this, item -> updateUI());
        mSlamProgress = view.findViewById(R.id.reconSlamProgress);
        mReconVM.getSlamProgress().observe(this, progress -> mSlamProgress.setText(progress + " %"));
        mNextButton = view.findViewById(R.id.reconNextButton);
        mNextButton.setOnClickListener(v -> Navigation.findNavController(v).navigate(R.id.action_reconstructionFragment_to_viewerFragment));
        mBackButton = view.findViewById(R.id.reconBackButton);
        mBackButton.setOnClickListener(v -> Navigation.findNavController(v).navigate(R.id.action_reconstructionFragment_to_cameraFragment));
    }

    private void updateUI() {

    }
}