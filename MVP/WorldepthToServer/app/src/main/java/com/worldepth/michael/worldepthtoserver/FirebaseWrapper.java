package com.worldepth.michael.worldepthtoserver;

import android.os.Environment;
import android.support.annotation.NonNull;
import android.util.Log;
import android.net.Uri;

//Firebase imports needed
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.storage.FirebaseStorage;
import com.google.firebase.storage.StorageReference;
import com.google.firebase.storage.UploadTask;

//Easy interaction with our database
public class FirebaseWrapper {
    //Debug TAG
    private static final String TAG = "worldepth";

    private FirebaseDatabase mDatabase; //Instance of database
    private StorageReference mStorageRef;

    public FirebaseWrapper() { //Constructor
        mDatabase = FirebaseDatabase.getInstance();
        mStorageRef = FirebaseStorage.getInstance().getReference();
    }

    //Get firebase database object
    public FirebaseDatabase getFirebaseDatabase() {
        return mDatabase;
    }

    //Get firebase storageReference object
    public StorageReference getStorageReference() { return mStorageRef; }

    //Write to the firebase database with serializable data
    public void writeToDatabase(String location, Object message) {
        DatabaseReference myRef = mDatabase.getReference(location); //location for message
        myRef.setValue(message); //sending the "message" object
        Log.d(TAG,"Wrote to Database");
    }


    //upload a file object to our Firebase Cloud Storage
    public void uploadFile(Uri file) {
        final StorageReference fileRef = mStorageRef.child("Bursts"); //Path to where files are placed

        fileRef.putFile(file)
                .addOnSuccessListener(new OnSuccessListener<UploadTask.TaskSnapshot>() {
                    @Override
                    public void onSuccess(UploadTask.TaskSnapshot taskSnapshot) {
                        // Get a URL to the uploaded content
                        com.google.android.gms.tasks.Task<android.net.Uri> downloadUrl = fileRef.getDownloadUrl();
                        Log.d(TAG, downloadUrl.toString());
                    }
                })
                .addOnFailureListener(new OnFailureListener() {
                    @Override
                    public void onFailure(@NonNull Exception exception) {
                        // Handle unsuccessful uploads
                        Log.d(TAG, "Error: " + exception.getMessage());
                    }
                });
    }
}