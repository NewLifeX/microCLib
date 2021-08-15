import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.format.DateTimeFormatter;

public class Project
{
    public static void main(String[] args)
    {
        System.out.printf("hello world!");

        FileInputStream fl;
        byte[] bin;
        try
        {
            fl = new FileInputStream("C:\\Users\\JiuHuan\\Desktop\\锁控板\\NewLCB48A_V1.0\\app.bin");
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

        System.out.printf("new BinHelper");
        var bh = new BinHelper(bin);

        // var fm = new SimpleDateFormat();
        // try {
        //     var a = fm.parse(time);
        // }
        // catch (ParseException ex)
        // {
        //     System.out.printf("%s",ex.toString());
        // }

    }
}
