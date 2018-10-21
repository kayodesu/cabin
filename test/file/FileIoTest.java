package file;

import java.io.FileInputStream;
import java.io.FileNotFoundException;

public class FileIoTest {
    
    public static void main(String[] args) throws Exception {
        new FileIoTest().fileNotFoundException();
    }

    public void fileNotFoundException() {
        try {
            FileInputStream fis = new FileInputStream("a/b/foo.txt");
        } catch (FileNotFoundException e) {
//            assertEquals("a/b/foo.txt (No such file or directory)", e.getMessage());
        }
    }
    
}
