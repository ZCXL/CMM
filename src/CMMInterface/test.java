import cmminterface.CMMInterface;
public class test{
	static{
		System.load("/home/zhuchao/Documents/C/CMMInterface/cmminterface/libcmm.so");
	}
	public static void main(String argv[]){
		CMMInterface c=new CMMInterface();
		c.compile("/home/zhuchao/Documents/CMM/test");
		int state=c.getState();
		System.out.println("State:"+String.valueOf(state));
		if(state==0){
			c.run("/home/zhuchao/Documents/CMM/test");
		}
	}
}