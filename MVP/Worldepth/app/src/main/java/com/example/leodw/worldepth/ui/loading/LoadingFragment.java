package com.example.leodw.worldepth.ui.loading;

import android.databinding.DataBindingUtil;
import android.databinding.BindingAdapter;
import com.example.leodw.worldepth.databinding.LoadingFragmentBinding;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;
import android.widget.Button;

import com.example.leodw.worldepth.R;
import com.example.leodw.worldepth.ui.MainActivity;

import androidx.navigation.Navigation;


public class LoadingFragment extends Fragment {

    private static final String TAG = "LoadingFragment";

    private LoadingViewModel mLoadingViewModel;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        LoadingFragmentBinding loadingFragmentBinding = DataBindingUtil.inflate(inflater, R.layout.loading_fragment, container, false);
        View view = loadingFragmentBinding.getRoot();
        loadingFragmentBinding.setViewModel(new LoadingViewModel());
        loadingFragmentBinding.executePendingBindings();

        Button loadingNextButton = view.findViewById(R.id.loadingNextButton);
        loadingNextButton.setOnClickListener((view1) -> {
            Navigation.findNavController(view1).navigate(R.id.action_loadingFragment_to_viewerFragment);
        });
        return view;
    }
}
