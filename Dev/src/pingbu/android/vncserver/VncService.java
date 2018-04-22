package pingbu.android.vncserver;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class VncService extends Service {
    static {
        System.loadLibrary("VncServer");
    }

    private static native void runServer();

    private final Runnable mServerProc = new Runnable() {
        @Override
        public void run() {
            runServer();
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        new Thread(mServerProc).start();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
