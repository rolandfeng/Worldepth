package com.example.leodw.worldepth.ui.signup.StartScreen;

import android.arch.lifecycle.ViewModelProviders;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import com.example.leodw.worldepth.R;
import com.example.leodw.worldepth.data.FirebaseWrapper;
import com.example.leodw.worldepth.ui.MainActivity;
import com.example.leodw.worldepth.ui.signup.Name.NameViewModel;
import com.example.leodw.worldepth.ui.signup.Phone.PhoneFragment;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.navigation.Navigation;
import androidx.navigation.fragment.FragmentNavigator;

public class StartScreenFragment extends Fragment {
    private static final String TAG = "StartScreenFragment";

    private StartScreenViewModel mViewModel;
    private FirebaseWrapper mFb;

    private FragmentNavigator.Extras mAnimExtras;

    public static StartScreenFragment newInstance() {
        return new StartScreenFragment();
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.start_screen_fragment, container, false);
        return view;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(StartScreenViewModel.class);
        mFb = ((MainActivity)this.getActivity()).getFirebaseWrapper();
        // TODO: Use the ViewModel
    }

    @Override
    public void onViewCreated(final View view, Bundle savedInstanceState) {
        createSharedElementTransitionFromStartToStartSignup();
        Button goToSignIn = view.findViewById(R.id.goToSignIn);
        goToSignIn.setOnClickListener((view1) -> {
            Navigation.findNavController(view1).navigate(R.id.action_startScreenFragment_to_signUpFragment);
        });

        Button goToSignUp = view.findViewById(R.id.goToSignUp);
        goToSignUp.setOnClickListener((view2) -> {
            Navigation.findNavController(getView()).navigate(R.id.action_startScreenFragment_to_startSignupFragment,
                    null,
                    null,
                    mAnimExtras);
        });

        Button goToCamera = view.findViewById(R.id.goToCamera);
        goToCamera.setOnClickListener((view4) -> {
            ((MainActivity) getActivity()).setLoginState(true);
            Navigation.findNavController(view4).navigate(R.id.action_startScreenFragment_to_cameraFragment);
        });
    }

    private void createSharedElementTransitionFromStartToStartSignup() {
        ImageView logo = getView().findViewById(R.id.start_logo);
        mAnimExtras = new FragmentNavigator.Extras.Builder()
                .addSharedElement(logo, "bigLogo")
                .build();
    }
}
