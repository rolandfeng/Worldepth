package com.example.leodw.worldepth.ui.signup.StartSignup;

import android.arch.lifecycle.ViewModelProviders;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.leodw.worldepth.R;
import com.example.leodw.worldepth.data.FirebaseWrapper;
import com.example.leodw.worldepth.ui.MainActivity;
import com.example.leodw.worldepth.ui.signup.SignUpFragment;
import com.example.leodw.worldepth.ui.signup.SignUpViewModel;
import com.google.firebase.auth.FirebaseAuth;

public class StartSignupFragment extends Fragment {
    private static final String TAG = "StartSignupFragment";

    private StartSignupViewModel mViewModel;
    private FirebaseWrapper mFb;


    public static StartSignupFragment newInstance() {
        return new StartSignupFragment();
    }


    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.start_signup_fragment, container, false);

        Button emailSignup = view.findViewById(R.id.emailSignup);
        emailSignup.setOnClickListener((view1) -> {
                Toast.makeText(getActivity(), "Going to email sign up page", Toast.LENGTH_SHORT).show();
                ((MainActivity) getActivity()).setViewPager(3); //email page
        });

        Button phoneSignup = view.findViewById(R.id.phoneSignup);
        phoneSignup.setOnClickListener((view1) -> {
                Toast.makeText(getActivity(), "Going to phone sign up page", Toast.LENGTH_SHORT).show();
                ((MainActivity) getActivity()).setViewPager(5); //phone page
        });
        return view;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(StartSignupViewModel.class);
        mFb = ((MainActivity)this.getActivity()).getFirebaseWrapper();
        // TODO: Use the ViewModel
    }

    @Override
    public void onViewCreated(final View view, Bundle savedInstanceState) {

    }


}