using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Metadata;

namespace MobileShell.Classes
{
    public class BuildInfo
    {
        private static BuildInfo _buildInfo;

        private BuildInfo()
        {
            //Better way coming SOOOOOOON™

            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 8))
                Build = Build.Version1903;
            else if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 7))
                Build = Build.Version1809;
            else if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
                Build = Build.SpringCreators;
            else if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 5))
                Build = Build.FallCreators;
            else if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 4))
                Build = Build.Creators;
            else if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 3))
                Build = Build.Anniversary;
            else if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 2))
                Build = Build.Threshold2;
            else if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 1))
                Build = Build.Threshold1;
            else
                Build = Build.Unknown;
        }

        public static Build Build { get; private set; }

        public static BuildInfo RetrieveApiInfo() => _buildInfo ?? (_buildInfo = new BuildInfo());
    }

    public enum Build
    {
        /// <summary>
        /// Unknown
        /// </summary>
        Unknown = 0,

        /// <summary>
        /// 10240 Threshold 1
        /// </summary>
        Threshold1 = 1507,

        /// <summary>
        /// 10586 Threshold 2
        /// </summary>
        Threshold2 = 1511,

        /// <summary>
        /// 14393 Redstone 1
        /// </summary>
        Anniversary = 1607,

        /// <summary>
        /// 15063 Redstone 2
        /// </summary>
        Creators = 1703,

        /// <summary>
        /// 16299 Redstone 3
        /// </summary>
        FallCreators = 1709,

        /// <summary>
        /// 17134 Redstone 4
        /// </summary>
        SpringCreators = 1803,

        /// <summary>
        /// 17763 Redstone 5
        /// </summary>
        Version1809 = 1809,

        /// <summary>
        /// 18362 19 'h' 1
        /// </summary>
        Version1903 = 1903,
    }
}
