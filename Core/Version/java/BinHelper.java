import java.text.SimpleDateFormat;
import java.util.Date;

public class BinHelper
{
    public byte[] Bin;
    public int FwVersion;
    public int HwVersion;

    /**
     *
     * @param bin 固件数组
     */
    public BinHelper(byte[] bin)
    {
        Bin = bin;
    }

    /**
     * 通过标记字符串，获取完整字符串
     * @param head 标记头，字符串头，前缀
     * @return 返回完整字符串，过滤掉前缀自己
     */
    public String GetWithHead(String head)
    {
        var sub = head.getBytes();

        int idx = ArrayHelper.IndexOf(Bin,sub,0);
        if(idx<0)return  "";
        var s = ArrayHelper.GetCString(Bin,idx);
        while(s.length() == head.length())
        {
            idx = ArrayHelper.IndexOf(Bin,sub,idx+1);
            if(idx<0)return  "";
            s = ArrayHelper.GetCString(Bin,idx);
        }

        return s;
    }

    /**
     * 计算固件版本
     * @return 固件版本
     */
    public int GetFwVer()
    {
        String head = "FirmwareBuildTime/";
        // FirmwareBuildTime/Aug  4 2021,18:29:09
        var src = GetWithHead(head);
        var timestr = src.substring(head.length());
        var dtbase = new Date(2000-1900,0,1,0,0,0);

        var res = GetFwTime(timestr).getTime() - dtbase.getTime();
        return (int)(res/1000/60);
    }

    /**
     * 计算硬件版本
     * @return 硬件版本
     */
    public int GetHwVer()
    {
        String head = "HardwareTime/";
        // HardwareTime/2021-05-22
        var src = GetWithHead(head);
        var timestr = src.substring(head.length());
        var dtbase = new Date(2000-1900,0,1,0,0,0);

        var res = GetHwTime(timestr).getTime() - dtbase.getTime();
        return (int)(res/1000/60);
    }

    /**
     * 月份字符串转为月份数字【0，11】
     * @param str 月份字符串3字母表示
     * @return 月份数字【0，11】
     */
    private static int StrMon(String str)
    {
        String month_names = "JanFebMarAprMayJunJulAugSepOctNovDec";
        return month_names.indexOf(str)/3;
    }

    /**
     * 获取编译时间
     * @param str C语言中 "__DATE__ , __TIME__"格式 “Aug  4 2021,18:29:09”
     * @return 返回时间
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
     * 获取硬件时间
     * @param str yyyy-MM-dd 格式 “2021-05-22”
     * @return 返回时间
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
        System.out.printf("%s",timestr);

        time = "2021-05-22";
        sdf=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        timestr = sdf.format(BinHelper.GetHwTime(time));
        System.out.printf("%s",timestr);
    }
}
