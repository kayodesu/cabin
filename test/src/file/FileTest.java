package file;

import java.io.File;

/**
 * Status: Fail
 */
public class FileTest {
    
    public static void main(String[] args) {        
        File file = new File("test.txt");
        String p = file.getAbsolutePath();
        System.out.println(p);
    }
    
}
