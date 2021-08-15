import java.util.Arrays;

public class ArrayHelper
{
    /**
     * 搜索子数组
     * @param src 源数组
     * @param sub 搜索项
     * @param offset 搜索起始位置，
     * @return 返回匹配位置
     */
    public static int IndexOf(byte[] src,byte[] sub,int offset)
    {
        if(src == null)return  -1;
        if(sub == null)return  -1;
        if(src.length < sub.length)return  -1;
        if(sub.length < 1)return  -1;
        if(offset<0)return  -1;
        if(offset+sub.length > src.length)return  -1;

        int i,j;

        int maxidx = src.length - sub.length - offset + 1;

        for (i = offset;i<maxidx;i++)
        {
            if(sub[0] == src[i])
            {
                for (j = 1;j<sub.length;j++)
                {
                    if(sub[j]!=src[i+j])break;
                }
                if(j == sub.length)return i;
            }
        }

        return -1;
    }

    /**
     * 搜索字节位置
     * @param src 源数组
     * @param value 搜索值
     * @param offset 偏移量
     * @return 返回匹配位置
     */
    public static int IndexOf(byte[] src,byte value,int offset)
    {
        if(src == null)return  -1;
        if(offset<0)return  -1;

        for (int i = offset;i<src.length;i++)
        {
            if(value == src[i])
            {
                return i;
            }
        }

        return -1;
    }

    /**
     * 从数组指定位置获取c字符串。c字符串的约定：字符串以 0x00 结尾。
     * @param src 源数组
     * @param offset 位置偏移量
     * @return 返回字符串
     */
    public static String GetCString(byte[] src,int offset)
    {
        int end = IndexOf(src,(byte)0x00,offset);
        if(end < 0)return  "";
        var len = end-offset;

        if(len == 0)return "";
        var sub = Arrays.copyOfRange(src,offset,len);

        return new String(sub);
    }
}
