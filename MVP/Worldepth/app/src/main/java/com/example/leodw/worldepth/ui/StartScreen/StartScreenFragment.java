package com.example.leodw.worldepth.ui.StartScreen;

import android.arch.lifecycle.ViewModelProviders;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.VectorEnabledTintResources;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.example.leodw.worldepth.R;
import com.example.leodw.worldepth.data.FirebaseWrapper;
import com.example.leodw.worldepth.ui.MainActivity;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.AuthResult;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;

import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.navigation.NavOptions;
import androidx.navigation.Navigation;
import androidx.navigation.fragment.FragmentNavigator;

public class StartScreenFragment extends Fragment {
    private static final String TAG = "StartScreenFragment";

    private Button mTestUserLogin;
    private EditText mEmailInput;
    private EditText mPasswordInput;

    private StartScreenViewModel mViewModel;
    private FirebaseWrapper mFb;

    private FragmentNavigator.Extras mAnimExtras;
    private NavOptions mNavOptions;

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
        mFb = ((MainActivity) this.getActivity()).getFirebaseWrapper();
        // TODO: Use the ViewModel
    }

    @Override
    public void onViewCreated(final View view, Bundle savedInstanceState) {
        createSignupTransitions();
        mEmailInput = view.findViewById(R.id.enterEmail);
        mPasswordInput = view.findViewById(R.id.enterPassword);
        mTestUserLogin = view.findViewById(R.id.testUserLogin);
        mTestUserLogin.setOnClickListener(v -> {
            login("JohnDoe@gmail.com", "Qwerty123!");
        });

        Button goToSignUp = view.findViewById(R.id.goToSignUp);
        goToSignUp.setOnClickListener((view2) -> {
            Navigation.findNavController(getView()).navigate(R.id.action_startScreenFragment_to_startSignupFragment,
                    null,
                    null,
                    mAnimExtras);
        });

        Button logIn = view.findViewById(R.id.loginBtn);
        logIn.setOnClickListener((view4) -> {
            login(mEmailInput.getText().toString(), mPasswordInput.getText().toString());
        });
    }

    private void createSignupTransitions() {
        ImageView logo = getView().findViewById(R.id.start_logo);
        mAnimExtras = new FragmentNavigator.Extras.Builder()
                .addSharedElement(logo, "bigLogo")
                .build();
        mNavOptions = new NavOptions.Builder()
                .setEnterAnim(R.animator.signup_anim)
                .build();
    }

    private void login(String email, String password) {
        FirebaseAuth auth = mFb.getFirebaseAuth();
        auth.signInWithEmailAndPassword(email, password).
                addOnCompleteListener(task -> {
                    if (task.isSuccessful()) {
                        Navigation.findNavController(getView()).navigate(R.id.action_startScreenFragment_to_cameraFragment);
                    } else {
                        Log.w(TAG, "signInWithEmail:failure", task.getException());
                        Toast.makeText(getContext(), "Authentication failed.", Toast.LENGTH_SHORT).show();
                    }
                });
    }
}