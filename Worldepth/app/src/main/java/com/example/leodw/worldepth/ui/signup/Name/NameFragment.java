package com.example.leodw.worldepth.ui.signup.Name;

import android.arch.lifecycle.ViewModelProviders;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import com.example.leodw.worldepth.R;
import com.example.leodw.worldepth.data.DataPair;
import com.example.leodw.worldepth.data.DataTransfer;
import com.example.leodw.worldepth.data.FirebaseWrapper;
import com.example.leodw.worldepth.ui.MainActivity;
import com.example.leodw.worldepth.ui.signup.Phone.PhoneFragment;
import com.example.leodw.worldepth.ui.signup.Phone.PhoneViewModel;

import androidx.navigation.Navigation;

public class NameFragment extends Fragment {
    private static final String TAG = "NameFragment";

    private NameViewModel mViewModel;
    private FirebaseWrapper mFb;
    private DataTransfer mDt;
    private EditText mFirstName;
    private EditText mLastName;

    private ImageView mNameBackButton;

    public static NameFragment newInstance() {
        return new NameFragment();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.name_fragment, container, false);
        mDt = ((MainActivity) this.getActivity()).getDataTransfer();

        Button nameNextButton = view.findViewById(R.id.nameNextButton);
        nameNextButton.setOnClickListener((view1) -> {
            mDt.addData(new DataPair(mFirstName.getText().toString(), "passwordFragment", "nameFragment"));
            mDt.addData(new DataPair(mLastName.getText().toString(), "passwordFragment", "nameFragment"));
            Navigation.findNavController(view1).navigate(R.id.action_nameFragment_to_birthdayFragment);
        });

        mNameBackButton = view.findViewById(R.id.nameBackButton);
        mNameBackButton.setOnClickListener((view2) -> {
            Navigation.findNavController(view2).popBackStack();
        });
        return view;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(NameViewModel.class);
        mFb = ((MainActivity)this.getActivity()).getFirebaseWrapper();
        // TODO: Use the ViewModel
    }

    @Override
    public void onViewCreated(final View view, Bundle savedInstanceState) {
        mFirstName = view.findViewById(R.id.firstName);
        mLastName = view.findViewById(R.id.lastName);
        mFirstName.requestFocus();
    }
}