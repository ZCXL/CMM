package cmminterface;
public class CMMInterface
{
	/* compile file and link file */
	public native static int compile(String path);
	/* run code */
	public native static void run(String path);
	/* get compile information */
	public native static String getResult();
	/* get compile state */
	public native static int getState();
}