import java.util.*;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.midlet.MIDlet;

public class Stopky extends MIDlet implements CommandListener, Runnable {

private Form frm;
private TextField time;
private long start;
private boolean canRun = false;
private Command ss;

public Stopky() {
	frm = new Form("Stopky");

	frm.append(time = new TextField("Čas", "00:00.00", 8,
			       	TextField.UNEDITABLE));

//	frm.addCommand(new Command("Reset", Command.CANCEL, 1));
	frm.addCommand(ss = new Command("Start", Command.ITEM, 1));
	frm.setCommandListener(this);
}

public void startApp() {
	Display.getDisplay(this).setCurrent(frm);
}

public void pauseApp() {
}

public void destroyApp(boolean unconditional) {
	canRun = false;
}

public void commandAction(Command c, Displayable d) {
	if (c == ss) {
		canRun = !canRun;
		if (canRun)
			startThread();
		frm.removeCommand(ss);
		frm.addCommand(ss = new Command(canRun ? "Stop" : "Start",
					Command.ITEM, 1));
	}
}
private void startThread() {
	start = new Date().getTime();

	new Thread(this).start();
}

public void run() {
	long diff, m, s, ms;

	while (canRun) {
		diff = new Date().getTime() - start;
		m = diff / 60000 % 60;
		s = diff / 1000 % 60;
		ms = diff / 10 % 100;

		time.setString((m <= 9 ? "0" : "") + m + ":" +
				(s <= 9 ? "0" : "") + s + "." +
				(ms <= 9 ? "0" : "") + ms);
		try {
			Thread.sleep(30);
		} catch (InterruptedException e) {
		}
	}
}

}
