package file;

import java.io.IOException;
import java.io.RandomAccessFile;

public class RandomAccessFileTest {

    public static void main(String[] args) throws Exception {
        RandomAccessFileTest test = new RandomAccessFileTest();
        test.writeTest();
        test.lengthTest();
        test.readTest();
        test.seekTest();
    }

    public void writeTest() throws IOException {
        try (RandomAccessFile randomAccessFile = new RandomAccessFile("RandomAccessFileWriteTest", "rw")) {
            //write data
            randomAccessFile.writeBytes("hello");
            randomAccessFile.writeInt(54);
            randomAccessFile.writeBytes("world");
            randomAccessFile.writeInt(-57);
        }
    }

    public void readTest() throws IOException {
        try (RandomAccessFile randomAccessFile = new RandomAccessFile("RandomAccessFileWriteTest", "rw")) {
            byte data[] = new byte[5];
            randomAccessFile.read(data);
            String result = new String(data);
//            Assert.assertTrue(result.equals("hello"));
            int intValue = randomAccessFile.readInt();
//            Assert.assertTrue(intValue == 54);
            randomAccessFile.read(data);
            result = new String(data);
//            Assert.assertTrue(result.equals("world"));
            intValue = randomAccessFile.readInt();
//            Assert.assertTrue(intValue == -57);
        }
    }

    public void seekTest() throws Exception {
        RandomAccessFile randomAccessFile = new RandomAccessFile("RandomAccessFileWriteTest", "rw");
        //write data
        randomAccessFile.writeBytes("hello");
        randomAccessFile.writeInt(54);
        randomAccessFile.writeBytes("world");
        randomAccessFile.writeInt(-57);
        randomAccessFile.seek(9);
        byte data[] = new byte[5];
        randomAccessFile.read(data);
        String result = new String(data);
        long seek = randomAccessFile.getFilePointer();
//        Assert.assertTrue(seek == 14);
//        Assert.assertTrue(result.equals("world"));
        randomAccessFile.close();
    }

    public void lengthTest() throws Exception {
        RandomAccessFile randomAccessFile = new RandomAccessFile("RandomAccessFileWriteTest", "rw");
        //write data
        randomAccessFile.writeBytes("hello");
        randomAccessFile.writeInt(54);
        randomAccessFile.writeBytes("world");
        randomAccessFile.writeInt(-57);

        randomAccessFile.setLength(5457);
        long length = randomAccessFile.length();
        //Assert.assertTrue(length == 5457);
//        Assert.assertTrue(length == 18);
        randomAccessFile.close();
    }

}
