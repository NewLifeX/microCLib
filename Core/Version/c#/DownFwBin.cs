using NewLCB.Messge;
using NewLife;
using NewLife.Data;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace NewLCB
{
    public class DownFwBin
    {
        public byte[] Bin;
        public DateTime FwBuildTime = new DateTime();
        public DateTime HardwareTime = new DateTime();

        public UInt32 FwVersion
        {
            get
            {
                var sub = new DateTime(2000, 1, 1);
                var dd = FwBuildTime - sub;

                return (UInt32)dd.TotalMinutes;
            }
        }

        public UInt32 HardwareVersion
        {
            get
            {
                var sub = new DateTime(2000, 1, 1);
                var dd = HardwareTime - sub;

                return (UInt32)dd.TotalMinutes;
            }
        }

        private void GetInfo()
        {
            // "FirmwareBuildTime/Aug  5 2021,18:10:35"
            var mask = "FirmwareBuildTime/";
            var str = GetStrWithHead(mask);
            if (str != null)
            {
                str = str.CutStart(mask);
                FwBuildTime = DateTime.Parse(str);
            }

            // "HardwareTime/2021-05-22"
            mask = "HardwareTime/";
            str = GetStrWithHead(mask);
            if (str != null)
            {
                str = str.CutStart(mask);
                HardwareTime = DateTime.Parse(str);
            }
        }

        public DownFwBin(byte[] bin)
        {
            Bin = bin;

            GetInfo();
        }

        public DownFwBin(string file)
        {
            if (!File.Exists(file)) return;

            Bin = File.ReadAllBytes(file);
            Address = 0xff;

            GetInfo();
        }

        /// <summary>固件大小</summary>
        /// <returns></returns>
        public long GetSize()
        {
            return Bin.Length;
        }

        /// <summary>获取特定标记头的字符串。其本身不算。</summary>
        /// <param name="head"></param>
        /// <returns></returns>
        private String GetStrWithHead(string head)
        {
            // 去掉c字符串的末尾。并作为数组进行后续搜索。
            var head2 = head.GetBytes(Encoding.ASCII).ReadBytes(0, head.Length - 1);

            var pkt = new Packet(Bin);
            var idx = pkt.IndexOf(head2);
            if (idx < 0) return null;
            var end = pkt.IndexOf(new byte[] { 0x00 }, idx);
            var str = pkt.ReadBytes(idx, end - idx).ToStr(Encoding.ASCII);

            // 固件里面有一份头，用于搜索新固件的内容。 所以必须要确认长度必须大于才行。
            if (str.Length == head.Length)
            {
                idx = pkt.IndexOf(head2, idx + str.Length);
                if (idx < 0) return null;
                end = pkt.IndexOf(new byte[] { 0x00 }, idx);
                str = pkt.ReadBytes(idx, end - idx).ToStr(Encoding.ASCII);
            }

            return str;
        }

        /// <summary>读取一部分</summary>
        /// <param name="offset"></param>
        /// <param name="len"></param>
        /// <returns></returns>
        public byte[] ReadSection(int offset, long len)
        {
            if (offset > Bin.Length) return new byte[0];
            if (offset + len > Bin.Length) len = Bin.Length - offset;

            return Bin.ReadBytes(offset, (int)len);
        }
    }
}
