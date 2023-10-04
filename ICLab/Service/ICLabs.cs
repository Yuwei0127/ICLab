using PCSC.Iso7816;
using PCSC;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Runtime.Remoting.Messaging;

namespace ICLab.Service
{
    public class ICLabs
    {
        public static void Test()
        {
            //讀卡機名稱
            string cReader;
            //尋找本機讀卡機設備
            using (var oReaders = PCSC.ContextFactory.Instance.Establish(PCSC.SCardScope.User))
            {
                cReader = oReaders.GetReaders().FirstOrDefault();
                if (string.IsNullOrEmpty(cReader))
                { //找不到任何PSCS讀卡機就跳走
                    Console.WriteLine("# 系統找不到任何讀卡機，請重新檢查硬體。");
                    return;
                }
                else
                {
                    Console.Clear();
                    Console.WriteLine($"# 歡迎使用台灣全民健保卡檢視程式 / by slashview.com");
                    Console.WriteLine($"# 共找到「{oReaders.GetReaders().Count()} 部設備」並使用「{cReader}」讀卡機，程式運行期間請勿任意移除設備。");
                }
            }
            //建立事件監控
            using (var oMonitor = PCSC.Monitoring.MonitorFactory.Instance.Create(PCSC.SCardScope.System))
            {
                oMonitor.CardRemoved += (oSender, oArgs) => { Console.WriteLine("# 偵測到晶片卡移除。"); };
                oMonitor.CardInserted += (oSender, oArgs) =>
                {
                    Console.WriteLine("# 偵測到晶片卡插入。");
                    GetCardInfo2(cReader);  //讀取健保卡顯性資料
                };
                oMonitor.MonitorException += (oSender, oArgs) =>
                {
                    Console.WriteLine("# 讀卡機被移除或是讀取晶片卡出現異常，請重新啟動程式。");
                    System.Environment.Exit(0); //強制退出
                };
                oMonitor.Start(cReader);
                //有可能執行程式前讀卡機與卡片就都已經準備好，如此一來並不會觸發事件，因此先強制執行一次讀取看看
                try
                { GetCardInfo2(cReader); }
                catch
                { } //若有讀取出錯就直接跳過（可能是未插卡）
                    //設定離開程序
                System.ConsoleKeyInfo oKey;
                do
                {
                    Console.WriteLine("# 若需要離開程式，請按下ESC按鈕。");
                    oKey = Console.ReadKey(true);
                } while (oKey.Key != System.ConsoleKey.Escape);
            }
            //程式結束
            Console.WriteLine("# 程式結束。");
        }

        /// <summary>
        /// 讀取台灣全民健保卡顯性資料
        /// </summary>
        public static void GetCardInfo(string cReader)
        {
            using (var oContext = PCSC.ContextFactory.Instance.Establish(PCSC.SCardScope.User))
            using (var oReader = new PCSC.Iso7816.IsoReader(
              context: oContext,
              readerName: cReader,
              mode: PCSC.SCardShareMode.Shared,
              protocol: PCSC.SCardProtocol.Any
            ))
            {
                Console.WriteLine("－－－－－");



                //初始化健保卡
                var oAdpuInit = new PCSC.Iso7816.CommandApdu(PCSC.Iso7816.IsoCase.Case4Short, oReader.ActiveProtocol)
                {
                    CLA = 0x00,
                    INS = 0xA4,
                    P1 = 0x04,
                    P2 = 0x00,
                    Data = new byte[] { 0xD1, 0x58, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11 }
                };

                ////初始化CDC
                //var oAdpuInit = new PCSC.Iso7816.CommandApdu(PCSC.Iso7816.IsoCase.Case4Short, oReader.ActiveProtocol)
                //{
                //    CLA = 0x00,
                //    INS = 0xA4,
                //    P1 = 0x04,
                //    P2 = 0x00,
                //    Data = new byte[] { 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31 }
                //};

                Console.WriteLine($"@ APDU InitCard:   {System.BitConverter.ToString(oAdpuInit.ToArray())}");

                //取得初始化健保卡回應
                var oAdpuInitResponse = oReader.Transmit(oAdpuInit);

                //檢查回應是否正確（144；00）
                if (!(oAdpuInitResponse.SW1.Equals(144) && oAdpuInitResponse.SW2.Equals(0)))
                {
                    Console.WriteLine("－－－－－");
                    Console.WriteLine("# 晶片卡並非健保卡，請換張卡片試看看。");
                    return;
                }

                //讀取健保顯性資訊
                var oAdpuProfile = new PCSC.Iso7816.CommandApdu(PCSC.Iso7816.IsoCase.Case4Short, oReader.ActiveProtocol)
                {
                    CLA = 0x00,
                    INS = 0xCA,
                    P1 = 0x11,
                    P2 = 0x00,
                    Data = new byte[] { 0x00, 0x00 }
                };

                ////讀取健保顯性資訊
                //var oAdpuProfile = new PCSC.Iso7816.CommandApdu(PCSC.Iso7816.IsoCase.Case4Short, oReader.ActiveProtocol)
                //{
                //    CLA = 0x00,
                //    INS = 0xCA,
                //    P1 = 0x11,
                //    P2 = 0x00,
                //    Le = 0x00
                //    // Data = new byte[] { 0x00, 0xCA, 0x11, 0x00, 0x00 }
                //};

                Console.WriteLine($"@ APDU GetProfile: {System.BitConverter.ToString(oAdpuProfile.ToArray())}");
                //取得讀取健保卡顯性資訊回應
                var oAdpuProfileResponse = oReader.Transmit(oAdpuProfile);

                //檢查回應是否正確（144；00）
                if (!(oAdpuInitResponse.SW1.Equals(144) && oAdpuInitResponse.SW2.Equals(0)))
                {
                    Console.WriteLine("－－－－－");
                    Console.WriteLine("# 健保卡讀取錯誤，請換張卡片試看看。");
                    return;
                }

                //如果有回應且具備資料的話，就將其輸出到畫面上
                if (oAdpuProfileResponse.HasData)
                {
                    //播放提示音
                    Console.Beep();

                    //位元組資料
                    byte[] aryData = oAdpuProfileResponse.GetData();

                    //文字編碼解碼器
                    var oUTF8 = System.Text.Encoding.UTF8;
                    var oBIG5 = System.Text.Encoding.GetEncoding("big5");

                    //建立使用者匿名物件
                    var oUser = new
                    {
                        cCardNumber = oUTF8.GetString(aryData.Take(12).ToArray()),
                        cName = oBIG5.GetString(aryData.Skip(12).Take(20).ToArray()),
                        cID = oUTF8.GetString(aryData.Skip(32).Take(10).ToArray()),
                        cBirthday = oUTF8.GetString(aryData.Skip(42).Take(7).ToArray()),
                        cGender = oUTF8.GetString(aryData.Skip(49).Take(1).ToArray()) == "M" ? "男" : "女",
                        cCardPublish = oUTF8.GetString(aryData.Skip(50).Take(7).ToArray())
                    };

                    //輸出資料
                    Console.WriteLine("－－－－－");
                    Console.WriteLine($"卡號　　：{oUser.cCardNumber}");
                    Console.WriteLine($"姓名　　：{oUser.cName}");
                    Console.WriteLine($"身分證號：{oUser.cID}");
                    Console.WriteLine($"生日　　：{oUser.cBirthday}");
                    Console.WriteLine($"性別　　：{oUser.cGender}");
                    Console.WriteLine($"發卡日期：{oUser.cCardPublish}");
                    Console.WriteLine("－－－－－");
                }
            }
        }


        public static byte[] HexStringToBytes(string hexString)
        {
            hexString = hexString.Replace(" ", "").Replace("\n", "").Replace("\r", "").Replace("\t", "");
            int length = hexString.Length;
            byte[] bytes = new byte[length / 2];
            for (int i = 0; i < length; i += 2)
            {
                bytes[i / 2] = Convert.ToByte(hexString.Substring(i, 2), 16);
            }
            return bytes;
        }


        public static void GetCardInfo2(string cReader)
        {
            using (var oContext = PCSC.ContextFactory.Instance.Establish(PCSC.SCardScope.User))
            using (var reader = new IsoReader(oContext, cReader, SCardShareMode.Shared, SCardProtocol.T1))
            {
                // Initialize CDC card
                var apduInit = new CommandApdu(IsoCase.Case4Short, reader.ActiveProtocol)
                {
                    CLA = 0x00,
                    INS = 0xCA,
                    P1 = 0x11,
                    P2 = 0x00,
                    Data = new byte[] { 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31 }
                };

                var initResponse = reader.Transmit(apduInit);

                if ((initResponse.SW1 == 0x90 && initResponse.SW2 == 0x00).Equals(false))
                {
                    Console.WriteLine("Failed to initialize CDC card.");
                    return;
                }




                // Now you can send other APDU commands to read CDC card data
                // For example, read basic information
                var apduReadData = new CommandApdu(IsoCase.Case4Short, reader.ActiveProtocol)
                {
                    CLA = 0x00,
                    INS = 0xCA,
                    P1 = 0x04,  // 指定数据对象的标识符，根据智能卡应用而定
                    P2 = 0x00,  // 请求全部数据
                    Data = new byte[] { },  // 无额外数据
                    Le = 0x00  // 无额外数据
                };

                var readDataResponse = reader.Transmit(apduReadData);

                if (readDataResponse.SW1 == 0x90 && readDataResponse.SW2 == 0x00)
                {
                    // Successfully read data, handle the response as needed
                    byte[] responseData = readDataResponse.GetData();
                    Console.WriteLine($"Data read successfully: {BitConverter.ToString(responseData)}");
                }
                else
                {
                    Console.WriteLine("Failed to read data.");
                }

                Console.WriteLine("CDC card initialized successfully.");
            }

            // =============================================================================================================================
        }

    }
}