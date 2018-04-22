package pingbu.android.vncserver;

import android.app.Application;
import android.content.Intent;

public class VncApp extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        startService(new Intent(this, VncService.class));
    }
}
