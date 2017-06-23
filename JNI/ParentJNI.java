public class ParentJNI {
	static {
		System.loadLibrary("ParentJNI");
	}

	private native void sayHello();

	public static void main(String[] args) {
		new ParentJNI().sayHello();
	}
}