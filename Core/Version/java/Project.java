import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class Project
{
    public static void main(String[] args)
    {
        System.out.printf("hello world!\r\n");

        String filename = "C:/Users/JiuHuan/Desktop/LCB/app.bin";
        FileInputStream fl;
        byte[] bin;
        try
        {
            fl = new FileInputStream(filename);
            bin = fl.readAllBytes();
        }
        catch (FileNotFoundException ex)
        {
            System.out.printf("open fail");
            return;
        }
        catch(IOException ex)
        {
            System.out.printf("read fail");
            return;
        }

        System.out.printf("Read Bin File %s true\r\n",filename);
        System.out.printf("new BinHelper\r\n");
        var bh = new BinHelper(bin);

        System.out.printf("HwVersion %d\r\n",bh.HwVersion);
        System.out.printf("FwVersion %d\r\n",bh.FwVersion);

        // BinHelper.Test();
    }
}
