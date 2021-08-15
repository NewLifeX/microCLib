import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * BIN固件帮助类。帮助获取BIN固件内的软硬件时间信息。进而计算出版本。
 * 版本计算算法。时间 减去 2000-1-1，0：0：0 得到的分钟数。
 */
public class BinHelper
{
    /**
     * 固件
     */
    public byte[] Bin;
    /**
     * 固件（软件）版本
     */
    public int FwVersion;
    /**
     * 硬件版本
     */
    public int HwVersion;

    /**
     * 初始化
     * @param bin
     */
    public BinHelper(byte[] bin)
    {
        Bin = bin;
        FwVersion = GetFwVer();
        HwVersion = GetHwVer();
    }

    /**
     * 按照给定的头搜索字符串
     * @param head
     * @return
     */
    public String GetWithHead(String head)
    {
        var sub = head.getBytes();

        int idx = ArrayHelper.IndexOf(Bin,sub,0);
        if(idx<0)return  "";
        var s = ArrayHelper.GetCString(Bin,idx);

        while(s.length() <= head.length()+2)
        {
            idx = ArrayHelper.IndexOf(Bin,sub,idx+1);
            if(idx<0)return  "";
            s = ArrayHelper.GetCString(Bin,idx);
        }

        return s;
    }

    /**
     * 获取固件版本
     * @return
     */
    public int GetFwVer()
    {
        String head = "FirmwareBuildTime/";
        // FirmwareBuildTime/Aug  4 2021,18:29:09
        var src = GetWithHead(head);
        System.out.printf("%s\r\n",src);
        var timestr = src.substring(head.length());
        var dtbase = new Date(2000-1900,0,1,0,0,0);

        var res = GetFwTime(timestr).getTime() - dtbase.getTime();
        return (int)(res/1000/60);
    }

    /**
     * 获取硬件版本
     * @return
     */
    public int GetHwVer()
    {
        String head = "HardwareTime/";
        // HardwareTime/2021-05-22
        var src = GetWithHead(head);
        System.out.printf("%s\r\n",src);
        var timestr = src.substring(head.length());
        var dtbase = new Date(2000-1900,0,1,0,0,0);

        var res = GetHwTime(timestr).getTime() - dtbase.getTime();
        return (int)(res/1000/60);
    }

    /**
     * 文字的月份转为0-11的月份值。
     * @param str Jan，Feb，Mar，Apr，May，Jun，Jul，Aug，Sep，Oct，Nov，Dec
     * @return 0-11
     */
    private static int StrMon(String str)
    {
        String month_names = "JanFebMarAprMayJunJulAugSepOctNovDec";
        return month_names.indexOf(str)/3;
    }

    /**
     * Aug  4 2021,18:29:09 格式转为 Date
     * @param str C语言中 __DATE__ , __TIME__
     * @return
     */
    public static Date GetFwTime(String str)
    {
        // Aug  4 2021,18:29:09
        var strs = str.split(" +|,|:");

        var dt = new Date(0,0,0,0,0,0);

        dt.setYear(Integer.valueOf(strs[2])-1900);
        dt.setMonth(StrMon(strs[0]));
        dt.setDate(Integer.valueOf(strs[1]));
        dt.setHours(Integer.valueOf(strs[3]));
        dt.setMinutes(Integer.valueOf(strs[4]));
        dt.setSeconds(Integer.valueOf(strs[5]));

        return  dt;
    }

    /**
     * 2021-05-22 格式转为时间 Date
     * @param str yyyy-MM-dd
     * @return
     */
    public static Date GetHwTime(String str)
    {
        // 2021-05-22
        var strs = str.split("-");

        var dt = new Date(0,0,0,0,0,0);

        dt.setYear(Integer.valueOf(strs[0])-1900);
        dt.setMonth(Integer.valueOf(strs[1])-1);
        dt.setDate(Integer.valueOf(strs[2]));
        if(strs.length>3)
        {
            dt.setHours(Integer.valueOf(strs[3]));
            dt.setMinutes(Integer.valueOf(strs[4]));
            dt.setSeconds(Integer.valueOf(strs[5]));
        }

        return  dt;
    }

    public static void Test()
    {
        System.out.printf("BinHelper.Test\r\n");

        var time = "Aug  4 2021,18:29:09";
        SimpleDateFormat sdf=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        var timestr = sdf.format(BinHelper.GetFwTime(time));
        System.out.printf("%s\r\n",timestr);

        time = "2021-05-22";
        sdf=new SimpleDateFormat("yyyy-MM-dd");
        timestr = sdf.format(BinHelper.GetHwTime(time));
        System.out.printf("%s\r\n",timestr);
    }
}
