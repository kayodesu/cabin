package file;

import java.io.FileInputStream;
import java.io.FileNotFoundException;

/**
 * Status: Fail
 */
public class FileNotFoundTest {
    
    public static void main(String[] args) {
        new FileNotFoundTest().fileNotFoundException();
    }

    public void fileNotFoundException() {
        try {
            FileInputStream fis = new FileInputStream("a/b/foo.txt");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }
    
}
