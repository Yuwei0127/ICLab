using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using System.Web;

namespace ICLab.Service
{
    public class ICService
    {
        public struct SCARD_IO_REQUEST
        {
            /// <summary>
            /// T0=1
            /// T1=2
            /// raw=0x00010000
            /// </summary>
            public int dwProtocol;
            /// <summary>
            /// 字节长度
            /// </summary>
            public int cbPciLength;
        }

        /// <summary>
        /// apdu命令返回命令
        /// </summary>
        public struct apdu_rs
        {
            public byte[] data;
            public byte sw1;
            public byte sw2;
        }


        /// <summary>
        /// 在操作系统上分配资源，建立将在其中进行设备数据库操作的资源管理器上下文
        /// </summary>
        /// <param name="dwScope">
        /// 输入类型；表示资源管理器上下文范围，
        /// 将是下列值之一
        /// SCARD_SCOPE_USER = 0;（在用户域中完成设备数据库操作）
        /// SCARD_SCOPE_SYSTEM =2;（在系统域中完成设备数据库操作）
        /// </param>
        /// <param name="pvReserved1">输入类型；保留，必须为NULL</param>
        /// <param name="pvReserved2">输入类型；保留，必须为NULL</param>
        /// <param name="phContext">输出类型；建立的资源管理器上下文的句柄</param>
        /// <returns>
        /// 成功 将返回 SCARD_S_SUCCESS = 0;
        /// </returns>
        [DllImport("WinSCard.dll")]
        public static extern uint SCardEstablishContext(uint dwScope, int pvReserved1, int pvReserved2, out ulong phContext);
        [DllImport("WinSCard.dll")]
        public static extern uint SCardEstablishContext(uint dwScope, int pvReserved1, int pvReserved2, out uint phContext);

        /// <summary>
        /// 列出系统中安装的读卡器的名字
        /// </summary>
        /// <param name="hContext">建立的资源管理器上下文的句柄，不能为NULL</param>
        /// <param name="mszGroups">输入类型；读卡器组名，为NULL时，表示列出所有读卡器</param>
        /// <param name="mszReaders">输出类型；系统中安装的读卡器的名字，各个名字之间用’\0’分隔，最后一个名字后面为两个连续的’\0’</param>
        /// <param name="pcchReaders">输入输出类型；mszReaders的长度</param>
        /// <returns>成功 将返回 SCARD_S_SUCCESS = 0;</returns>
        [DllImport("WinSCard.dll")]
        public static extern uint SCardListReaders(ulong hContext, string mszGroups, byte[] mszReaders, ref uint pcchReaders);
        [DllImport("WinSCard.dll")]
        private static extern uint SCardListReaders(uint hContext, string mszGroups, byte[] mszReaders, ref uint pcchReaders);

        /// <summary>
        /// 在应用程序与读卡器上的智能卡之间建立一个连接
        /// </summary>
        /// <param name="hContext">建立的资源管理器上下文的句柄</param>
        /// <param name="cReaderName">输入类型；包含智能卡的读卡器名称（读卡器名称由ScardListReaders()给出）</param>
        /// <param name="dwShareMode">
        /// 输入类型；应用程序对智能卡的操作方式
        /// SCARD_SHARE_SHARED  =2；（多个应用共享同一个智能卡）
        /// SCARD_SHARE_EXCLUSIVE = 1；（应用独占智能卡）
        /// SCARD_SHARE_DIRECT = 3；（应用将智能卡作为私有用途，直接操纵智能卡，不允许其它应用访问智能卡）
        /// </param>
        /// <param name="dwPrefProtocol">
        /// 输入类型；连接使用的协议
        /// SCARD_PROTOCOL_T0 = 1;（使用T=0协议）
        /// SCARD_PROTOCOL_T1 = 2;（使用T=1协议）
        /// </param>
        /// <param name="phCard">输出类型；与智能卡连接的句柄</param>
        /// <param name="ActiveProtocol">输出类型；实际使用的协议</param>
        /// <returns>如果返回成功 将返回 SCARD_S_SUCCESS = 0;</returns>
        [DllImport("WinSCard.dll")]
        public static extern uint SCardConnect(ulong hContext, byte[] cReaderName, int dwShareMode, int dwPrefProtocol, ref ulong phCard, ref uint ActiveProtocol);
        [DllImport("WinSCard.dll")]
        private static extern uint SCardConnect(uint hContext, byte[] cReaderName, int dwShareMode, int dwPrefProtocol, ref uint phCard, ref uint ActiveProtocol);

        /// <summary>
        /// 向智能卡发送指令，并接受返回的数据
        /// </summary>
        /// <param name="hCard">输入类型；与智能卡连接的句柄(由SCardConnect给出)</param>
        /// <param name="pioSendPci">输入类型；指令的协议头结构的指针，由SCARD_IO_REQUEST结构定义。后面是使用的协议的协议控制信息。一般使用系统定义的结构，SCARD_PCI_T0（T=0协议）、 SCARD_PCI_T1（T=1协议）、SCARD_PCI_RAW（原始协议）</param>
        /// <param name="pbSendbuff">输入类型；要发送到智能卡的数据的指针</param>
        /// <param name="cbSendLength">输入类型；pbSendBuffer的字节数目</param>
        /// <param name="pioRecvPci">输入输出类型；指令协议头结构的指针，后面是使用的协议的协议控制信息，如果不返回协议控制信息，可以为NULL</param>
        /// <param name="pbRecvbuff">输入输出类型；从智能卡返回的数据的指针</param>
        /// <param name="pcbRecvLength">输入输出类型；pbRecvBuffer的大小</param>
        /// <returns>如果返回成功 将返回 SCARD_S_SUCCESS = 0;</returns>
        [DllImport("WinSCard.dll")]
        private static extern uint SCardTransmit(ulong hCard, [In] ref SCARD_IO_REQUEST pioSendPci, byte[] pbSendbuff, uint cbSendLength, ref SCARD_IO_REQUEST pioRecvPci, [Out] byte[] pbRecvbuff, out uint pcbRecvLength);
        [DllImport("WinSCard.dll")]
        private static extern uint SCardTransmit(uint hCard, [In] ref SCARD_IO_REQUEST pioSendPci, byte[] pbSendbuff, uint cbSendLength, ref SCARD_IO_REQUEST pioRecvPci, [Out] byte[] pbRecvbuff, out uint pcbRecvLength);
        //public static extern int SCardTransmit(int hCard, ref SCARD_IO_REQUEST pioSendPci, ref byte pbSendBuffer, ref int cbSendLength, ref SCARD_IO_REQUEST pioRecvPci, ref byte pbRecvBuffer,ref int pcbRecvLength); 

        /// <summary>
        /// 终止应用与智能卡之间的连接
        /// </summary>
        /// <param name="hCard">输入类型；与智能卡连接的句柄</param>
        /// <param name="Disposition">
        /// 输入类型；断开连接时，对智能卡的操作
        /// SCARD_LEAVE_CARD = 0;对卡不做任何操作
        /// SCARD_RESET_CARD = 1;卡复位
        /// SCARD_UNPOWER_CARD = 2;对卡断电
        /// SCARD_EJECT_CARD = 3; 弹出卡
        /// </param>
        /// <returns>如果返回成功 将返回 SCARD_S_SUCCESS = 0;</returns>
        [DllImport("WinSCard.dll")]
        private static extern uint SCardDisconnect(ulong hCard, int Disposition);
        [DllImport("WinSCard.dll")]
        private static extern uint SCardDisconnect(uint hCard, int Disposition);

        /// <summary>
        /// 释放资源管理器的上下文
        /// </summary>
        /// <param name="phContext">输入类型；ScardEstablishContext()建立的资源管理器上下文的句柄，不能为NULL</param>
        /// <returns></returns>
        [DllImport("WinSCard.dll")]
        private static extern uint SCardReleaseContext(ulong phContext);
        [DllImport("WinSCard.dll")]
        private static extern uint SCardReleaseContext(uint phContext);



        // 成功
        public int SCARD_S_SUCCESS = 0;

        public int SCARD_SCOPE_USER = 0; //（在用户域中完成设备数据库操作）
        public int SCARD_SCOPE_SYSTEM = 2;//（在系统域中完成设备数据库操作）

        //（多个应用共享同一个智能卡）
        public int SCARD_SHARE_SHARED = 2;

        //（使用T=0协议）
        public int SCARD_PROTOCOL_T0 = 1;

        //（使用T=1协议）
        public int SCARD_PROTOCOL_T1 = 2;


    }
}