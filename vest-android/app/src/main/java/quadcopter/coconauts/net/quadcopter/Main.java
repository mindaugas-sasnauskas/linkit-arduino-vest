package quadcopter.coconauts.net.quadcopter;

import android.bluetooth.BluetoothAdapter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class Main extends ActionBarActivity {

    public final String TAG = "Main";

    private SeekBar elevation;
    private TextView debug;
    private TextView status;
    private TextView ddd;
    private TextView tsView;
    private TextView tlView;
    private TextView trView;
    private TextView taView;
    private ProgressBar progressBar;
    private Bluetooth bt;
//    private WebView webview;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setContentView(R.layout.activity_main);
        final WebView myWebView = (WebView) findViewById(R.id.webview);
        myWebView.loadUrl("https://vest-stats.herokuapp.com");
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
//        Button button = (Button) findViewById(R.id.reload);
//        button.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                webview.reload();
//            }
//        });
        debug = (TextView) findViewById(R.id.textDebug);
        status = (TextView) findViewById(R.id.textStatus);
        ddd = (TextView) findViewById(R.id.ddd);
        tsView = (TextView) findViewById(R.id.tsView);
        tlView = (TextView) findViewById(R.id.tlView);
        trView = (TextView) findViewById(R.id.trView);
        taView = (TextView) findViewById(R.id.taView);

        progressBar = (SeekBar) findViewById(R.id.seekBar);

        findViewById(R.id.restart).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                connectService();
            }
        });
        findViewById(R.id.buttonRefreh).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                bt.sendMessage("1");
            }
        });
        findViewById(R.id.reload).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                myWebView.reload();
            }
        });

        elevation = (SeekBar) findViewById(R.id.seekBar);
        elevation.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.d("Seekbar","onStopTrackingTouch ");
                int progress = seekBar.getProgress();
                String p = String.valueOf(progress);
                debug.setText(p);
                bt.sendMessage(p);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                Log.d("Seekbar","onStartTrackingTouch ");
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                //Log.d("Seekbar", "onProgressChanged " + progress);
            }
        });

        bt = new Bluetooth(this, mHandler);
        connectService();

    }

    public void connectService(){
        try {
            status.setText("Connecting...");
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            if (bluetoothAdapter.isEnabled()) {
                bt.start();
                bt.connectDevice("HC-06");
                Log.d(TAG, "Btservice started - listening");
                status.setText("Connected");
                ddd.setText("Connected");
            } else {
                Log.w(TAG, "Btservice started - bluetooth is not enabled");
                status.setText("Bluetooth Not enabled");
            }
        } catch(Exception e){
            Log.e(TAG, "Unable to start bt ",e);
            status.setText("Unable to connect " +e);
        }
    }

    
JSONArray data = null;
    private static final String TAG_DATA = "data";
    private static final String TAG_TS = "ts";
    private static final String TAG_T1 = "t1";
    private static final String TAG_T2 = "t2";
    private static final String TAG_T3 = "t3";
    private static final String TAG_T4 = "t4";
    private static final String TAG_T5 = "t5";
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Bluetooth.MESSAGE_STATE_CHANGE:
                    Log.d(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                    break;
                case Bluetooth.MESSAGE_WRITE:
                    Log.d(TAG, "MESSAGE_WRITE "+msg);
                    break;
                case Bluetooth.MESSAGE_READ:
                    Log.d(TAG,  "MESSAGE_READ "+ msg.obj);
                    String str = msg.obj.toString();
                    // str ="\"data\":[{\"t1\":\"20\",\"t2\":\"20\",\"t3\":\"20\",\"t4\":\"20\",\"t5\":\"20\",\"ts\":\"0\"}]";

                    if (str != null) {
                        try {
                            JSONObject jsonObj = new JSONObject(str);

                            // Getting JSON Array node
                            data = jsonObj.getJSONArray(TAG_DATA);

                            // looping through All Contacts
                            for (int i = 0; i < data.length(); i++) {
                                JSONObject c = data.getJSONObject(i);

                                int ts = c.getInt(TAG_TS);
                                int t1 = c.getInt(TAG_T1);
                                int t2 = c.getInt(TAG_T2);
                                int t3 = c.getInt(TAG_T3);
                                int t4 = c.getInt(TAG_T4);
                                int t5 = c.getInt(TAG_T5);

//String spee = ts.toString();
                                String tss = c.getString(TAG_TS);
                                String t1s = c.getString(TAG_T1);
                                String t2s = c.getString(TAG_T2);
                                String t3s = c.getString(TAG_T3);
                                String t4s = c.getString(TAG_T4);
                                String t5s = c.getString(TAG_T5);


                                // adding each child node to HashMap key => value
//                                data.put(TAG_TS, ts);
//                                data.put(TAG_T1, t1);
//                                data.put(TAG_T2, t2);
//                                data.put(TAG_T3, t3);
//                                data.put(TAG_T4, t4);
//                                data.put(TAG_T5, t5);

//                                Log.d("ddddd ","ff"+t3);
                                taView.setText(t3s);
                                tlView.setText(t2s);
                                trView.setText(t5s);
                                tsView.setText(tss);

                                int oldProgress  = progressBar.getProgress();
//                                String p = String.valueOf(progress);
                                if (ts != oldProgress) {
                                    progressBar.setProgress(ts);
                                }


                                if (tss.length()>0){
                                    String pp = String.valueOf(ts);
                                    ddd.setText(pp);
                                }

                            }
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                    } else {
                        Log.e("ServiceHandler", "Couldn't get any data from the url");
                    }


                    break;
                case Bluetooth.MESSAGE_DEVICE_NAME:
                    Log.d(TAG, "MESSAGE_DEVICE_NAME "+msg);
                    break;
                case Bluetooth.MESSAGE_TOAST:
                    Log.d(TAG, "MESSAGE_TOAST "+msg);
                    break;
            }
        }
    };

}
