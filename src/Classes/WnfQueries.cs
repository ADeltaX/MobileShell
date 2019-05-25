using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MobileShell.Classes
{
    public class WnfQueries
    {
        public const ulong WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED = 0xD83063EA3BF1C75;
        public const ulong WNF_SHEL_NOTIFICATIONS = 0xD83063EA3BC1035;
        public const ulong WNF_LFS_STATE = 0x41950821A3BC0875;
        public const ulong WNF_CELL_SIGNAL_STRENGTH_BARS_CAN0 = 0xD8A0B2EA3BC1075;
        public const ulong WNF_CELL_SIGNAL_STRENGTH_BARS_CAN1 = 0xD8A0B2EA3BD1075;
        public const ulong WNF_TMCN_ISTABLETMODE = 0xf850339a3bc0835;

        public static FocusAssistStatus ToFocusAssistStatus(byte[] data)
        {
            //THIS WORKS ONLY ON 17134+
            if (data[0] == 0x0)
                return FocusAssistStatus.Off;
            else if (data[0] == 0x1)
                return FocusAssistStatus.PriorityOnly;
            else if (data[0] == 0x2)
                return FocusAssistStatus.AlarmsOnly;
            else
                return FocusAssistStatus.Off; //illegal
        }

        public static bool ToBool(byte[] data) => BitConverter.ToBoolean(data, 0);
        public static int ToInt32(byte[] data) => BitConverter.ToInt32(data, 0);

        public static int QueryUnreadNotifications()
        {
            var status = QueryWnf(WNF_SHEL_NOTIFICATIONS);
            return BitConverter.ToInt32(status.Data, 0);
        }

        public static bool QueryIsLocationInUse()
        {
            var status = QueryWnf(WNF_LFS_STATE);
            return BitConverter.ToBoolean(status.Data, 0);
        }


        /// <summary>
        /// Get cellular signal strength for the first can (range 0 to 5)
        /// Returns -1 if there is no cellular
        /// </summary>
        /// <returns></returns>
        public static int QueryCan0CellularSignalStrength()
        {
            var x = QueryWnf(WNF_CELL_SIGNAL_STRENGTH_BARS_CAN0);
            if (x.Data.Length < 1)
                return -1;
            else
                return BitConverter.ToInt32(x.Data, 0);
        }

        /// <summary>
        /// Get cellular signal strength for the second can (range 0 to 5)
        /// Returns -1 if there is no cellular
        /// </summary>
        /// <returns></returns>
        public static int QueryCan1CellularSignalStrength()
        {
            var x = QueryWnf(WNF_CELL_SIGNAL_STRENGTH_BARS_CAN1);
            if (x.Data.Length < 1)
                return -1;
            else
                return BitConverter.ToInt32(x.Data, 0);
        }

        /// <summary>
        /// Get notified when a wnf state changes
        /// </summary>
        /// <param name="state">The wnf state to subscribe</param>
        /// <param name="callback">The callback to get notifications</param>
        public static void SubscribeWnf(ulong state, WnfUserCallback callback)
        {
            RtlSubscribeWnfStateChangeNotification(out IntPtr sub, state, 0,
                callback, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
        }

        [StructLayout(LayoutKind.Sequential)]
        private class WnfType
        {
            public Guid TypeId;
        }

        private class WnfStateData
        {
            public int Changestamp { get; }
            public byte[] Data { get; }

            public WnfStateData() { }
            public WnfStateData(int changestamp, byte[] data)
            {
                Changestamp = changestamp;
                Data = data;
            }
        }

        [DllImport("ntdll.dll")]
        private static extern int ZwQueryWnfStateData(
            ref ulong StateId,
            [In, Optional] WnfType TypeId,
            [Optional] IntPtr Scope,
            out int Changestamp,
            SafeBuffer DataBuffer,
            ref int DataBufferSize
        );

        [DllImport("ntdll.dll")]
        private static extern int ZwUpdateWnfStateData(
            ref ulong StateId,
            SafeBuffer DataBuffer,
            int DataBufferSize,
            [In, Optional] WnfType TypeId,
            [Optional] IntPtr Scope,
            int MatchingChangestamp,
            [MarshalAs(UnmanagedType.Bool)] bool CheckChangestamp
        );

        [DllImport("ntdll.dll")]
        private static extern int RtlSubscribeWnfStateChangeNotification(
            out IntPtr Subscription,
            ulong StateName,
            uint ChangeStamp,
            WnfUserCallback Callback,
            IntPtr CallbackContext,
            IntPtr TypeId,
            IntPtr Buffer,
            IntPtr Unknown
            );

        public delegate IntPtr WnfUserCallback(
            ulong StateName,
            uint ChangeStamp,
            IntPtr TypeId,
            IntPtr CallbackContext,
            IntPtr Buffer,
            uint BufferSize);

        // Original dev: James Forshaw @tyranid: Project Zero
        // Ref: https://github.com/googleprojectzero/sandbox-attacksurface-analysis-tools/blob/46b95cba8f76fae9a5c8258d13057d5edfacdf90/NtApiDotNet/SafeHandles.cs
        public class SafeHGlobalBuffer : SafeBuffer
        {
            public SafeHGlobalBuffer(int length)
              : this(length, length) { }

            protected SafeHGlobalBuffer(int allocation_length, int total_length)
                : this(Marshal.AllocHGlobal(allocation_length), total_length, true) { }

            public SafeHGlobalBuffer(IntPtr buffer, int length, bool owns_handle)
              : base(owns_handle)
            {
                Length = length;
                Initialize((ulong)length);
                SetHandle(buffer);
            }

            public static SafeHGlobalBuffer Null { get { return new SafeHGlobalBuffer(IntPtr.Zero, 0, false); } }

            protected override bool ReleaseHandle()
            {
                if (!IsInvalid)
                {
                    Marshal.FreeHGlobal(handle);
                    handle = IntPtr.Zero;
                }
                return true;
            }

            public byte[] ReadBytes(ulong byte_offset, int count)
            {
                byte[] ret = new byte[count];
                ReadArray(byte_offset, ret, 0, count);
                return ret;
            }

            public byte[] ReadBytes(int count)
            {
                return ReadBytes(0, count);
            }

            public SafeHGlobalBuffer(byte[] data) : this(data.Length)
            {
                Marshal.Copy(data, 0, handle, data.Length);
            }

            public int Length
            {
                get; private set;
            }
        }


        private static int UpdateWnf(ulong state, byte[] data)
        {
            using (var buffer = data.ToBuffer())
            {
                ulong state_name = state;

                return ZwUpdateWnfStateData(ref state_name, buffer,
                    buffer.Length, null, IntPtr.Zero, 0, false);
            }
        }

        private static WnfStateData QueryWnf(ulong state)
        {
            var data = new WnfStateData();
            int tries = 10;
            int size = 4096;
            while (tries-- > 0)
            {
                using (SafeHGlobalBuffer buffer = new SafeHGlobalBuffer(size))
                {
                    int status;
                    status = ZwQueryWnfStateData(ref state, null, IntPtr.Zero, out int changestamp, buffer, ref size);

                    if (status == 0xC0000023)
                        continue;
                    data = new WnfStateData(changestamp, buffer.ReadBytes(size));
                }
            }
            return data;
        }
    }

    static class BufferUtils
    {
        public static WnfQueries.SafeHGlobalBuffer ToBuffer(this byte[] value) => new WnfQueries.SafeHGlobalBuffer(value);
    }

    public enum FocusAssistStatus
    {
        Off = 0,
        PriorityOnly = 1,
        AlarmsOnly = 2
    }
}
