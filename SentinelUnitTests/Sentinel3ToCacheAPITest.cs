using Aristocrat.OASIS.Business.SpeedMediaEvents;
using SpeedMediaClient;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using System;
using System.Collections.Generic;
using System.ServiceModel;

namespace SentinelUnitTests
{


    /// <summary>
    ///This is a test class for Sentinel3ToCacheAPITest and is intended
    ///to contain all Sentinel3ToCacheAPITest Unit Tests
    ///</summary>
    [TestClass()]
    public class Sentinel3ToCacheAPITest
    {


        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        /// <summary>
        /// Helper routine for formatting exception strings.
        ///</summary>
        string ExceptionString(Exception ex, string desc)
        {
            return desc + ": Type=" + ex.GetType().ToString() + " Msg=" + ex.Message;
        }

        #region Additional test attributes
        // 
        //You can use the following additional attributes as you write your tests:
        //
        //Use ClassInitialize to run code before running the first test in the class
        //[ClassInitialize()]
        //public static void MyClassInitialize(TestContext testContext)
        //{
        //}
        //
        //Use ClassCleanup to run code after all tests in a class have run
        //[ClassCleanup()]
        //public static void MyClassCleanup()
        //{
        //}
        //
        //Use TestInitialize to run code before running each test
        //[TestInitialize()]
        //public void MyTestInitialize()
        //{
        //}
        //
        //Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //public void MyTestCleanup()
        //{
        //}
        //
        #endregion


        /// <summary>
        ///A test for SetBindingType
        ///</summary>
        [TestMethod()]
        public void SetBindingTypeTest()
        {
            try
            {
                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                Sentinel3ToCacheAPI.BindingType bindingType = Sentinel3ToCacheAPI.BindingType.Https;
                target.SetBindingType(bindingType);
            }
            catch (Exception ex)
            {
                Assert.Fail(ex.Message);
            }
        }

        /// <summary>
        ///A test for SetBindingType
        ///</summary>
        [TestMethod()]
        public void SetBindingTypeTestPrivate2()
        {
            try
            {

                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                var po = new PrivateObject(target);

                {
                    // Test setting BindingType.
                    const Sentinel3ToCacheAPI.BindingType bindingTypeExpected = Sentinel3ToCacheAPI.BindingType.Http;
                    target.SetBindingType(bindingTypeExpected);
                    // Get private value and convert to type used for private value.
                    Sentinel3ToCacheAPI.BindingType bindingType =
                        (Sentinel3ToCacheAPI.BindingType)po.GetField("_bindingType");
                    if (bindingTypeExpected != bindingType)
                    {
                        Assert.Fail("_bindingType {0} != {1}", bindingType, bindingTypeExpected);
                    }
                }

                {
                    // Get code coverage for changing BindingType.
                    const Sentinel3ToCacheAPI.BindingType bindingTypeExpected = Sentinel3ToCacheAPI.BindingType.Https;
                    target.SetBindingType(bindingTypeExpected);
                    // Get private value and convert to type used for private value.
                    Sentinel3ToCacheAPI.BindingType bindingType =
                        (Sentinel3ToCacheAPI.BindingType)po.GetField("_bindingType");
                    if (bindingTypeExpected != bindingType)
                    {
                        Assert.Fail("_bindingType {0} != {1}", bindingType, bindingTypeExpected);
                    }
                }
            }
            catch (Exception ex)
            {
                Assert.Fail(ex.Message);
            }
        }

        /// <summary>
        /// A test for open connection throwing an exception due to no endpoint listening.
        /// Try it again to cover code for closing comms.
        ///</summary>
        [TestMethod()]
        public void SetHostOpenTest()
        {
            try
            {
                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                string host = "invalid.com";
                string functionInfo = string.Empty;
                target.SetHost(ref host, ref functionInfo);
                bool goodException = false;
                try
                {
                    int major, minor, revision, build;

                    // Verify that the next method call throws the following exception.
                    //    "There was no endpoint listening at [...]"
                    target.GetSpeedMediaVersion(out major, out minor, out revision, out build, ref functionInfo);
                    Assert.Fail("Expected exception opening host, but exception did not occur");
                }
                catch (System.ServiceModel.EndpointNotFoundException)
                {
                    // The preferred exception for correct behavior.
                    goodException = true;
                }
                catch (System.TimeoutException)
                {
                    // This exception is also allowed to occur for correct behavior, but is not preferred.
                    goodException = true;
                }
                catch (System.ServiceModel.CommunicationException)
                {
                    // This exception is also allowed, but could indicate many different problems.
                    goodException = true;
                }
                catch (Exception ex)
                {
                    Assert.Fail(ExceptionString(ex, "Unexpected exception setting/opening host (try1)"));
                }

                if (goodException)
                {
                    // Good.  Caught an exception we wanted.

                    // Try it again to cover code for closing comms.
                    try
                    {
                        int major, minor, revision, build;
                        // Verify that the next method call throws the following exception.
                        //    "There was no endpoint listening at [...]"
                        target.GetSpeedMediaVersion(out major, out minor, out revision, out build, ref functionInfo);
                        Assert.Fail("Expected exception opening host, but exception did not occur");
                    }
                    catch (System.ServiceModel.EndpointNotFoundException)
                    {
                        // The preferred exception for correct behavior.
                    }
                    catch (System.TimeoutException)
                    {
                        // This exception is also allowed to occur for correct behavior, but is not preferred.
                    }
                    catch (System.ServiceModel.CommunicationException)
                    {
                        // This exception is also allowed, but could indicate many different problems.
                        goodException = true;
                    }
                    catch (Exception ex)
                    {
                        Assert.Fail(ExceptionString(ex, "Unexpected exception setting/opening host (try2)"));
                    }
                }
            }
            catch (Exception ex)
            {
                Assert.Fail(ExceptionString(ex, "Unexpected exception setting/opening host"));
            }
        }

        /// <summary>
        /// A test for open connection throwing an exception due to BindingType.
        ///</summary>
        [TestMethod()]
        public void SetHostOpenTest2()
        {
            try
            {
                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                target.SetBindingType(Sentinel3ToCacheAPI.BindingType.Http);
                string host = "invalid.com";
                string functionInfo = string.Empty;
                target.SetHost(ref host, ref functionInfo);
                try
                {
                    int major, minor, revision, build;

                    // Verify that the next method call throws the following exception.
                    //    "There was no endpoint listening at [...]"
                    target.GetSpeedMediaVersion(out major, out minor, out revision, out build, ref functionInfo);
                    Assert.Fail("Expected exception opening host, but exception did not occur");
                }
                catch (System.ArgumentOutOfRangeException)
                {
                    // Good.  Caught the one we want.
                }
                catch (Exception ex)
                {
                    Assert.Fail("Unexpected exception setting/opening host: " + ex.Message);
                }
            }
            catch (Exception ex)
            {
                Assert.Fail("Unexpected exception setting/opening host: " + ex.Message);
            }
        }

        /// <summary>
        ///A test for SetHost
        ///</summary>
        [TestMethod()]
        public void SetHostTestPrivate()
        {
            try
            {
                string localHostExpected = "127.0.0.1";
                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                var po = new PrivateObject(target);
                string functionInfo = string.Empty;
                target.SetHost(ref localHostExpected, ref functionInfo);

                // Get private value and convert to type used for private value.
                string localHost = (string)po.GetField("_cacheHost");
                if (localHostExpected != localHost)
                {
                    Assert.Fail("_cacheHost {0} != {1}", localHost, localHostExpected);
                }
            }
            catch (Exception ex)
            {
                Assert.Fail("Exception setting host: " + ex.Message);
            }
        }

        /// <summary>
        ///A test for CloseConnection
        ///</summary>
        [TestMethod()]
        public void CloseConnectionTest()
        {
            try
            {
                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                target.CloseConnection();
            }
            catch (Exception ex)
            {
                Assert.Fail("Exception closing connection: " + ex.Message);
            }
        }

        /// <summary>
        /// Tests for Sentinel3ToCacheClient constructors
        ///</summary>
        [TestMethod()]
        public void Sentinel3ToCacheClientTest()
        {
            try
            {
                SpeedMediaClient.SpeedMediaCacheReference.Sentinel3ToCacheClient scc;

                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                target.SetBindingType(Sentinel3ToCacheAPI.BindingType.Https);
                string host = "invalid.com";
                string functionInfo = string.Empty;
                target.SetHost(ref host, ref functionInfo);

                // Get private values.
                var po = new PrivateObject(target);
                BasicHttpBinding bindingType = (BasicHttpBinding)po.Invoke("GetHttpsBinding");
                EndpointAddress endpointAddress = (EndpointAddress)po.Invoke("GetEndpointAddress");

                try
                {
                    scc = new SpeedMediaClient.SpeedMediaCacheReference.Sentinel3ToCacheClient();
                }
                catch (System.InvalidOperationException)
                {
                    // Good.  Caught the one we want.
                }
                catch (Exception ex)
                {
                    Assert.Fail("Unexpected exception in constructor(): " + ex.Message);
                }

                try
                {
                    scc = new SpeedMediaClient.SpeedMediaCacheReference.Sentinel3ToCacheClient("");
                }
                catch (System.InvalidOperationException)
                {
                    // Good.  Caught the one we want.
                }
                catch (Exception ex)
                {
                    Assert.Fail("Unexpected exception in constructor(configName): " + ex.Message);
                }

                try
                {
                    scc = new SpeedMediaClient.SpeedMediaCacheReference.Sentinel3ToCacheClient("", "invalid.com");
                }
                catch (System.UriFormatException)
                {
                    // Good.  Caught the one we want.
                }
                catch (Exception ex)
                {
                    Assert.Fail("Unexpected exception in constructor(configName, address): " + ex.Message);
                }

                try
                {
                    scc = new SpeedMediaClient.SpeedMediaCacheReference.Sentinel3ToCacheClient("", endpointAddress);
                }
                catch (System.InvalidOperationException)
                {
                    // Good.  Caught the one we want.
                }
                catch (Exception ex)
                {
                    Assert.Fail("Unexpected exception in constructor(configName, endpoint): " + ex.Message);
                }

                try
                {
                    scc = new SpeedMediaClient.SpeedMediaCacheReference.Sentinel3ToCacheClient(bindingType, endpointAddress);
                }
                catch (Exception ex)
                {
                    Assert.Fail("Unexpected exception in constructor(binding, endpoint): " + ex.Message);
                }
            }
            catch (Exception ex)
            {
                Assert.Fail("Unexpected exception: " + ex.Message);
            }
        }

        /// <summary>
        ///A test for a CustomMessageLine constructor
        ///</summary>
        [TestMethod()]
        public void CustomMessageLineTest()
        {
            const string text0 = "Text0";
            const string text1 = "Text1";
            CustomMessageLine customMessageLines0 = new CustomMessageLine();
            CustomMessageLine customMessageLines1;
            customMessageLines0.Text = text0;
            customMessageLines0.JustificationType = EventData.CustomMessageJustificationType.Right;
            customMessageLines0.Y1 = 11;
            customMessageLines0.Y2 = 12;
            customMessageLines0.X1 = 13;
            customMessageLines0.X2 = 14;
            customMessageLines0.FontName = "Arial";
            customMessageLines0.ForegroundColor = 0x010203;
            customMessageLines0.Bold = false;
            customMessageLines0.Underline = false;
            customMessageLines0.DropShadow = true;
            customMessageLines0.FontSize = 8;

            customMessageLines1 = new CustomMessageLine(customMessageLines0, text1);

            Assert.AreEqual(customMessageLines1.Text, text1, "Text");
            Assert.AreEqual(customMessageLines1.JustificationType, customMessageLines1.JustificationType, "JustificationType");
            Assert.AreEqual(customMessageLines1.Y1, customMessageLines0.Y1, "Y1");
            Assert.AreEqual(customMessageLines1.Y2, customMessageLines0.Y2, "Y2");
            Assert.AreEqual(customMessageLines1.X1, customMessageLines0.X1, "X1");
            Assert.AreEqual(customMessageLines1.X2, customMessageLines0.X2, "X2");
            Assert.AreEqual(customMessageLines1.ForegroundColor, customMessageLines0.ForegroundColor, "ForegroundColor");
            Assert.AreEqual(customMessageLines1.Bold, customMessageLines0.Bold, "Bold");
            Assert.AreEqual(customMessageLines1.Underline, customMessageLines0.Underline, "Underline");
            Assert.AreEqual(customMessageLines1.DropShadow, customMessageLines0.DropShadow, "DropShadow");
            Assert.AreEqual(customMessageLines1.FontSize, customMessageLines0.FontSize, "FontSize");
            Assert.AreEqual(customMessageLines1.FontName, customMessageLines0.FontName, "FontName");
        }

        /// <summary>
        ///A test for ConvertMessageScreenToStruct
        ///</summary>
        [TestMethod()]
        public void ConvertMessageScreenToStructTestNull()
        {
            CustomMessageScreenStruct screenStruct;

            const string screenName = "xx";
            CustomMessageScreen screen = new CustomMessageScreen();
            screen.MessageLines = null;
            screen.ScreenType = null;

            screenStruct = Sentinel3ToCacheAPI.ConvertMessageScreenToStruct(ref screen, screenName);
            Assert.AreEqual(screenStruct.ScreenType, null, "Need screenStruct.ScreenType=null");
            Assert.AreEqual(screenStruct.MessageLines, null, "Need screenStruct.MessageLines=null");
        }

        /// <summary>
        ///A test for ConvertMessageScreenToStruct
        ///</summary>
        [TestMethod()]
        public void ConvertMessageScreenToStructTest0()
        {
            CustomMessageScreenStruct screenStruct;

            const string screenName = "xx";
            CustomMessageScreen screen = new CustomMessageScreen();
            const string screenType = "Basic";
            screen.ScreenType = screenType;
            screen.MessageLines = null;

            screenStruct = Sentinel3ToCacheAPI.ConvertMessageScreenToStruct(ref screen, screenName);
            Assert.AreEqual(screenStruct.ScreenType.Rank, 1, "Need screenStruct.ScreenType.Rank=1");
            Assert.AreEqual(screenStruct.ScreenType.GetLength(0), 1, "Need screenStruct.ScreenType.GetLength=1");
            Assert.AreEqual(screenStruct.ScreenType[0], screenType, "Need screenStruct.ScreenType=" + screenType);
            Assert.AreEqual(screenStruct.MessageLines, null, "Need screenStruct.MessageLines=null");
        }

        /// <summary>
        ///A test for ConvertMessageScreenToStruct
        ///</summary>
        [TestMethod()]
        public void ConvertMessageScreenToStructTest1()
        {
            CustomMessageScreenStruct screenStruct;

            const string screenName = "xx";
            CustomMessageScreen screen = new CustomMessageScreen();
            const string screenType = "Trigger";
            screen.ScreenType = screenType;

            screen.MessageLines = new CustomMessageLines();
            CustomMessageLine[] customMessageLines = new CustomMessageLine[1];
            customMessageLines[0] = new CustomMessageLine();
            customMessageLines[0].Text = "Text0";
            customMessageLines[0].JustificationType = EventData.CustomMessageJustificationType.Right;
            customMessageLines[0].Y1 = 11;
            customMessageLines[0].Y2 = 12;
            customMessageLines[0].X1 = 13;
            customMessageLines[0].X2 = 14;
            customMessageLines[0].FontName = "Arial";
            customMessageLines[0].ForegroundColor = 0x010203;
            customMessageLines[0].Bold = false;
            customMessageLines[0].Underline = false;
            customMessageLines[0].DropShadow = true;
            customMessageLines[0].FontSize = 8;
            screen.MessageLines.Add(customMessageLines[0]);

            screenStruct = Sentinel3ToCacheAPI.ConvertMessageScreenToStruct(ref screen, screenName);
            Assert.AreEqual(screenStruct.ScreenType.Rank, 1, "Need screenStruct.ScreenType.Rank=1");
            Assert.AreEqual(screenStruct.ScreenType.GetLength(0), 1, "Need screenStruct.ScreenType.GetLength=1");
            Assert.AreEqual(screenStruct.ScreenType[0], screenType, "Need screenStruct.ScreenType=" + screenType);
            Assert.AreEqual(screenStruct.MessageLines.Rank, 1, "Need screenStruct.MessageLines.Rank=1");
            Assert.AreEqual(screenStruct.FontNames.Rank, 1, "Need screenStruct.FontNames.Rank=1");
            Assert.AreEqual(screenStruct.Texts.Rank, 1, "Need screenStruct.Texts.Rank=1");
            Assert.AreEqual(screenStruct.MessageLines.GetLength(0), customMessageLines.GetLength(0),
                "Need screenStruct.MessageLines.GetLength=" + customMessageLines.GetLength(0).ToString());
            Assert.AreEqual(screenStruct.FontNames.GetLength(0), customMessageLines.GetLength(0),
                "Need screenStruct.FontNames.GetLength=" + customMessageLines.GetLength(0).ToString());
            Assert.AreEqual(screenStruct.Texts.GetLength(0), customMessageLines.GetLength(0),
                "Need screenStruct.Texts.GetLength=" + customMessageLines.GetLength(0).ToString());

            for (int idx = 0; idx <= customMessageLines.GetUpperBound(0); ++idx)
            {
                string idxStr = " idx=" + idx.ToString();
                Assert.AreEqual((EventData.CustomMessageJustificationType)screenStruct.MessageLines[idx].JustificationType,
                    customMessageLines[idx].JustificationType, "JustificationType" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Y1, customMessageLines[idx].Y1, "Y1" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Y2, customMessageLines[idx].Y2, "Y2" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].X1, customMessageLines[idx].X1, "X1" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].X2, customMessageLines[idx].X2, "X2" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].ForegroundColor, customMessageLines[idx].ForegroundColor, "ForegroundColor" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Bold, customMessageLines[idx].Bold, "Bold" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Underline, customMessageLines[idx].Underline, "Underline" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].DropShadow, customMessageLines[idx].DropShadow, "DropShadow" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].FontSize, customMessageLines[idx].FontSize, "FontSize" + idxStr);

                Assert.AreEqual(screenStruct.FontNames[idx], customMessageLines[idx].FontName, "FontName" + idxStr);
                Assert.AreEqual(screenStruct.Texts[idx], customMessageLines[idx].Text, "Text" + idxStr);
            }
        }

        /// <summary>
        ///A test for ConvertMessageScreenToStruct
        ///</summary>
        [TestMethod()]
        public void ConvertMessageScreenToStructTest3()
        {
            CustomMessageScreenStruct screenStruct;

            const string screenName = "xx";
            CustomMessageScreen screen = new CustomMessageScreen();
            const string screenType = "Trigger";
            screen.ScreenType = screenType;

            screen.MessageLines = new CustomMessageLines();
            CustomMessageLine[] customMessageLines = new CustomMessageLine[3];
            customMessageLines[0] = new CustomMessageLine();
            customMessageLines[0].Text = "Text0";
            customMessageLines[0].JustificationType = EventData.CustomMessageJustificationType.Right;
            customMessageLines[0].Y1 = 11;
            customMessageLines[0].Y2 = 12;
            customMessageLines[0].X1 = 13;
            customMessageLines[0].X2 = 14;
            customMessageLines[0].FontName = "Arial";
            customMessageLines[0].ForegroundColor = 0x010203;
            customMessageLines[0].Bold = false;
            customMessageLines[0].Underline = false;
            customMessageLines[0].DropShadow = true;
            customMessageLines[0].FontSize = 8;
            screen.MessageLines.Add(customMessageLines[0]);

            customMessageLines[1] = new CustomMessageLine();
            customMessageLines[1].Text = "Text1";
            customMessageLines[1].JustificationType = EventData.CustomMessageJustificationType.Left;
            customMessageLines[1].Y1 = 21;
            customMessageLines[1].Y2 = 22;
            customMessageLines[1].X1 = 23;
            customMessageLines[1].X2 = 24;
            customMessageLines[1].FontName = "Courier New";
            customMessageLines[1].ForegroundColor = 0x111213;
            customMessageLines[1].Bold = false;
            customMessageLines[1].Underline = true;
            customMessageLines[1].DropShadow = false;
            customMessageLines[1].FontSize = 9;
            screen.MessageLines.Add(customMessageLines[1]);

            customMessageLines[2] = new CustomMessageLine();
            customMessageLines[2].Text = "Text2";
            customMessageLines[2].JustificationType = EventData.CustomMessageJustificationType.Left;
            customMessageLines[2].Y1 = 31;
            customMessageLines[2].Y2 = 32;
            customMessageLines[2].X1 = 33;
            customMessageLines[2].X2 = 34;
            customMessageLines[2].FontName = "System";
            customMessageLines[2].ForegroundColor = 0x212223;
            customMessageLines[2].Bold = true;
            customMessageLines[2].Underline = false;
            customMessageLines[2].DropShadow = false;
            customMessageLines[2].FontSize = 10;
            screen.MessageLines.Add(customMessageLines[2]);

            screenStruct = Sentinel3ToCacheAPI.ConvertMessageScreenToStruct(ref screen, screenName);
            Assert.AreEqual(screenStruct.ScreenType.Rank, 1, "Need screenStruct.ScreenType.Rank=1");
            Assert.AreEqual(screenStruct.ScreenType.GetLength(0), 1, "Need screenStruct.ScreenType.GetLength=1");
            Assert.AreEqual(screenStruct.ScreenType[0], screenType, "Need screenStruct.ScreenType=" + screenType);
            Assert.AreEqual(screenStruct.MessageLines.Rank, 1, "Need screenStruct.MessageLines.Rank=1");
            Assert.AreEqual(screenStruct.FontNames.Rank, 1, "Need screenStruct.FontNames.Rank=1");
            Assert.AreEqual(screenStruct.Texts.Rank, 1, "Need screenStruct.Texts.Rank=1");
            Assert.AreEqual(screenStruct.MessageLines.GetLength(0), customMessageLines.GetLength(0),
                "Need screenStruct.MessageLines.GetLength=" + customMessageLines.GetLength(0).ToString());
            Assert.AreEqual(screenStruct.FontNames.GetLength(0), customMessageLines.GetLength(0),
                "Need screenStruct.FontNames.GetLength=" + customMessageLines.GetLength(0).ToString());
            Assert.AreEqual(screenStruct.Texts.GetLength(0), customMessageLines.GetLength(0),
                "Need screenStruct.Texts.GetLength=" + customMessageLines.GetLength(0).ToString());

            for (int idx = 0; idx <= customMessageLines.GetUpperBound(0); ++idx)
            {
                string idxStr = " idx=" + idx.ToString();
                Assert.AreEqual((EventData.CustomMessageJustificationType)screenStruct.MessageLines[idx].JustificationType,
                    customMessageLines[idx].JustificationType, "JustificationType" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Y1, customMessageLines[idx].Y1, "Y1" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Y2, customMessageLines[idx].Y2, "Y2" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].X1, customMessageLines[idx].X1, "X1" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].X2, customMessageLines[idx].X2, "X2" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].ForegroundColor, customMessageLines[idx].ForegroundColor, "ForegroundColor" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Bold, customMessageLines[idx].Bold, "Bold" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].Underline, customMessageLines[idx].Underline, "Underline" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].DropShadow, customMessageLines[idx].DropShadow, "DropShadow" + idxStr);
                Assert.AreEqual(screenStruct.MessageLines[idx].FontSize, customMessageLines[idx].FontSize, "FontSize" + idxStr);

                Assert.AreEqual(screenStruct.FontNames[idx], customMessageLines[idx].FontName, "FontName" + idxStr);
                Assert.AreEqual(screenStruct.Texts[idx], customMessageLines[idx].Text, "Text" + idxStr);
            }
        }

        /// <summary>
        ///A test for ConvertJaggedByteArrayToTwoDimensionalByteArray
        ///</summary>
        [TestMethod()]
        public void ConvertJaggedByteArrayToTwoDimensionalByteArrayTest()
        {
            // inialize byte[2][3] array
            byte[][] jaggedArray;
            jaggedArray = new byte[2][];
            for (byte x = 0; x < 2; x++)
            {
                jaggedArray[x] = new byte[3];
                for (byte y = 0; y < 3; y++)
                {
                    jaggedArray[x][y] = (byte)((x + 3) * (y + 10));
                }
            }

            // declare two dimensional array
            byte[,] twoD;
            twoD = Sentinel3ToCacheAPI.ConvertJaggedByteArrayToTwoDimensionalByteArray(jaggedArray);

            // check returned array
            for (byte y = 0; y < 3; y++)
                for (byte x = 0; x < 2; x++)
                {
                    Assert.IsTrue(jaggedArray[x][y] == twoD[y, x], "Jagged array conversion failed");
                }

        }

        /// <summary>
        ///A test for SetHost
        ///</summary>
        [TestMethod()]
        public void SetHostTest()
        {
            try
            {
                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                string localHost = "127.0.0.1";
                string functionInfo = string.Empty;
                target.SetHost(ref localHost, ref functionInfo);
            }
            catch (Exception ex)
            {
                Assert.Fail("Exception setting host: " + ex.Message);
            }
        }

        /// <summary>
        ///A test for ConvertTwoDimensionalByteArrayToJaggedByteArray
        ///</summary>
        [TestMethod()]
        public void ConvertTwoDimensionalByteArrayToJaggedByteArrayTest()
        {
            // initialize two dimensional array
            byte[,] twoD = new byte[3, 2];
            for (byte y = 0; y < 3; y++)
                for (byte x = 0; x < 2; x++)
                {
                    twoD[y, x] = (byte)((x + 3) * (y + 10));
                }


            // declare jagged array
            byte[][] jaggedArray;

            jaggedArray = Sentinel3ToCacheAPI.ConvertTwoDimensionalByteArrayToJaggedByteArray(twoD);

            //// check jaggedArray
            int xBounds = jaggedArray.GetUpperBound(0);
            int yBounds = jaggedArray[0].GetUpperBound(0);
            Assert.IsTrue(xBounds == 1, "Jagged array 'x' is incorrect.");
            Assert.IsTrue(yBounds == 2, "Jagged array[0] is incorrect.");
            for (byte x = 0; x <= xBounds; x++)
            {
                for (byte y = 0; y <= yBounds; y++)
                {
                    Assert.AreEqual(jaggedArray[x][y], twoD[y, x], "Two dimentsional array conversion failed.");
                }
            }

        }

        /// <summary>
        /// Helper routine for converting DateTime to a byte array.
        ///</summary>
        void ConvertDateTimeToTimeStampArray(DateTime dateTime, out byte[] timeStamp)
        {
            timeStamp = new byte[16];
            timeStamp[0] = (byte)((dateTime.Year >> 0) & 0xff);
            timeStamp[1] = (byte)((dateTime.Year >> 8) & 0xff);
            timeStamp[2] = (byte)((dateTime.Month >> 0) & 0xff);
            timeStamp[3] = (byte)((dateTime.Month >> 8) & 0xff);
            timeStamp[4] = (byte)(((int)dateTime.DayOfWeek >> 0) & 0xff);
            timeStamp[5] = (byte)(((int)dateTime.DayOfWeek >> 8) & 0xff);
            timeStamp[6] = (byte)((dateTime.Day >> 0) & 0xff);
            timeStamp[7] = (byte)((dateTime.Day >> 8) & 0xff);
            timeStamp[8] = (byte)((dateTime.Hour >> 0) & 0xff);
            timeStamp[9] = (byte)((dateTime.Hour >> 8) & 0xff);
            timeStamp[10] = (byte)((dateTime.Minute >> 0) & 0xff);
            timeStamp[11] = (byte)((dateTime.Minute >> 8) & 0xff);
            timeStamp[12] = (byte)((dateTime.Second >> 0) & 0xff);
            timeStamp[13] = (byte)((dateTime.Second >> 8) & 0xff);
            timeStamp[14] = (byte)((dateTime.Millisecond >> 0) & 0xff);
            timeStamp[15] = (byte)((dateTime.Millisecond >> 8) & 0xff);
        }

        /// <summary>
        /// A simple coverage test.
        ///</summary>
        [TestMethod()]
        public void CoverageTest()
        {
            bool done = false;
            int funcNum = 0;
            string funcName = "n/a";

            try
            {
                Sentinel3ToCacheAPI target = new Sentinel3ToCacheAPI();
                target.SetBindingType(Sentinel3ToCacheAPI.BindingType.Https);
                string host = "invalid.com";
                string functionInfo = string.Empty;
                target.SetHost(ref host, ref functionInfo);

                // For each function ...
                // Verify that the method call throws the following exception.
                //    "There was no endpoint listening at [...]"
                funcNum = 0;
                do
                {
                    try
                    {
                        switch (funcNum)
                        {
                            case 0:
                                {
                                    funcName = "Checkin";
                                    int slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int lastSequenceNumber = 5, lastType = 6, lastID = 7;
                                    // Out parameters.
                                    byte[,] nextTimestamps;
                                    int[] nextSequenceNumbers, nextTypes, nextIDs;
                                    int foundCount;
                                    bool hasMore;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.Checkin(slotmastID,
                                        ref ocr,
                                        lastTimeStamp,
                                        ref lastTimeStampInString,
                                        lastSequenceNumber,
                                        lastType,
                                        lastID,
                                        out nextTimestamps,
                                        out nextSequenceNumbers,
                                        out nextTypes,
                                        out nextIDs,
                                        out foundCount,
                                        out hasMore,
                                        ref functionInfo);
                                }
                                break;

                            case 1:
                                {
                                    funcName = "GetEventStartNotification";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId, contentId;
                                    int[] triggerType;
                                    long[] triggerMin, triggerMax;
                                    int numTriggers, lastTriggeredEventID;
                                    bool includeCarded, includeUncarded, includeInRandomSelection, groupCumulative;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetEventStartNotification(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr, out eventId,
                                       out contentId, out triggerType, out triggerMin, out triggerMax,
                                       out numTriggers, out lastTriggeredEventID, out includeCarded,
                                       out includeUncarded, out includeInRandomSelection, out groupCumulative, ref functionInfo);
                                }
                                break;

                            case 2:
                                {
                                    funcName = "GetSplashdownTriggeredEventNotificationCustom";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId, triggeredEventId;
                                    int[] triggerType;
                                    long[] triggerMin, triggerMax;
                                    int numTriggers, duration;
                                    bool groupCumulative, pointsToBeAwarded;
                                    CustomMessageScreenStruct customScreen;
                                    CustomMessageScreenStruct customCelebrationScreen;
                                    CustomMessageScreenStruct customFinaleScreen;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetSplashdownTriggeredEventNotificationCustom(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr,
                                        out eventId, out triggeredEventId,
                                        out triggerType, out triggerMin,
                                        out triggerMax, out numTriggers, out duration,
                                        out groupCumulative, out pointsToBeAwarded,
                                        out  customScreen,
                                        out  customCelebrationScreen,
                                        out  customFinaleScreen,
                                        ref functionInfo);
                                }
                                break;

                            case 3:
                                {
                                    funcName = "GetSplashdownAwardNotificationCustom";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int triggeredEventID;
                                    bool showAward, isTriggerAward;
                                    int awardAmount;
                                    CustomMessageScreenStruct customScreen;
                                    string mediaData;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetSplashdownAwardNotificationCustom(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr,
                                                         out triggeredEventID, out showAward, out isTriggerAward,
                                                         out awardAmount, out customScreen, out mediaData, ref functionInfo);
                                }
                                break;

                            case 4:
                                {
                                    funcName = "GetRicochetTriggeredEventNotificationCustom";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId, triggeredEventID;
                                    bool show;
                                    CustomMessageScreenStruct triggerScreen;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetRicochetTriggeredEventNotificationCustom(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr,
                                                                out eventId, out triggeredEventID, out show,
                                                                out triggerScreen, ref functionInfo);
                                }
                                break;

                            case 5:
                                {
                                    funcName = "GetRicochetAwardNotificationCustom";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId;
                                    bool showAward;
                                    CustomMessageScreenStruct customScreen;
                                    string mediaData;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetRicochetAwardNotificationCustom(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr,
                                                       out eventId, out showAward,
                                                       out customScreen, out mediaData, ref functionInfo);
                                }
                                break;

                            case 6:
                                {
                                    funcName = "GetJackpotAnnouncementNotification";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId, triggeredEventID;
                                    bool carded;
                                    CustomMessageScreenStruct screen;
                                    bool showAward;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetJackpotAnnouncementNotification(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr,
                                                       out eventId, out triggeredEventID, out carded,
                                                       out screen, out showAward, ref functionInfo);
                                }
                                break;

                            case 7:
                                {
                                    funcName = "GetTriggerlessAnnouncementNotification";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId;
                                    bool carded;
                                    CustomMessageScreenStruct screen;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetTriggerlessAnnouncementNotification(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr,
                                                           out eventId, out carded, out screen, ref functionInfo);
                                }
                                break;

                            case 8:
                                {
                                    funcName = "GetTriggerRejectedNotification";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId, lastTriggeredEventId;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetTriggerRejectedNotification(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr, out eventId,
                                                   out lastTriggeredEventId, ref functionInfo);
                                }
                                break;

                            case 9:
                                {
                                    funcName = "GetSpeedMediaVersion";
                                    int major, minor, revision, build;
                                    functionInfo = string.Empty;
                                    target.GetSpeedMediaVersion(out major, out minor, out revision, out build, ref functionInfo);
                                }
                                break;

                            case 10:
                                {
                                    funcName = "GetSpeedMediaVersion";
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    int slotmastID = 456;
                                    bool hasPlayedGame = true;

                                    target.CardIn(ref ocr, slotmastID, hasPlayedGame, ref functionInfo);
                                }
                                break;

                            case 11:
                                {
                                    funcName = "CardOut";
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    int slotmastID = 456;

                                    target.CardOut(ref ocr, slotmastID, ref functionInfo);
                                }
                                break;

                            case 12:
                                {
                                    funcName = "GamePlayedAfterCardIn";
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    int slotmastID = 456;

                                    target.GamePlayedAfterCardIn(ref ocr, slotmastID, ref functionInfo);
                                }
                                break;

                            case 13:
                                {
                                    funcName = "RequestNonCardedEvents";
                                    int slotmastID = 456;
                                    functionInfo = string.Empty;
                                    target.RequestNonCardedEvents(slotmastID, ref functionInfo);
                                }
                                break;

                            case 14:
                                {
                                    funcName = "GetFlashStartAnnouncement";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    // Out parameters.
                                    string filename;
                                    int size, checksum, fileID;
                                    string ipAddress;
                                    int port, mediaType;
                                    string lastTimeStampInString = string.Empty;
                                    functionInfo = string.Empty;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetFlashStartAnnouncement(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, out filename, out size,
                                              out checksum, out fileID, out ipAddress, out port,
                                              out mediaType, ref functionInfo);
                                }
                                break;

                            case 15:
                                {
                                    funcName = "GetFlashUdpCompleteAnnouncement";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    // Out parameters.
                                    int fileID;
                                    string lastTimeStampInString = string.Empty;
                                    functionInfo = string.Empty;
                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetFlashUdpCompleteAnnouncement(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, out fileID, ref functionInfo);
                                }
                                break;

                            case 16:
                                {
                                    funcName = "GetFileInformation";
                                    string filename = "abc.def";
                                    functionInfo = string.Empty;
                                    // Out parameters.
                                    int filesize;

                                    target.GetFileInformation(ref filename, out filesize, ref functionInfo);
                                }
                                break;

                            case 17:
                                {
                                    funcName = "GetConfig";
                                    int slotmastID = 456;
                                    // Out parameters.
                                    int currentFileID, currentChecksum, pendingfileID, pendingChecksum;
                                    double par;
                                    functionInfo = string.Empty;
                                    target.GetConfig(slotmastID, out currentFileID, out currentChecksum, out pendingfileID,
                                        out pendingChecksum, out par, ref functionInfo);

                                }
                                break;

                            case 18:
                                {
                                    funcName = "GetEventXMLFile";
                                    // Out parameters.
                                    int fileID, checksum;
                                    functionInfo = string.Empty;
                                    target.GetEventXMLFile(out fileID, out checksum, ref functionInfo);
                                }
                                break;

                            case 19:
                                {
                                    funcName = "InEventTrigger";
                                    int eventID = 123;
                                    long jackpotAmount = 12345;
                                    DateTime dateTime0 = new DateTime(2000, 1, 2, 1, 23, 45, 678);
                                    DateTime dateTime1 = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] initialTimeStamp, currentTimeStamp;
                                    int slotMastId = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string initialTimeStampInString = string.Empty;
                                    string currentTimeStampInString = string.Empty;
                                    int lastTriggeredEventId = 121;
                                    long initialCoinIn = 1, initialCoinOut = 2, initialJackpotHandpay = 3;
                                    long initialExternalBonusingMachinePayout = 4, initialExternalBonusingAttendantPayout = 5;
                                    long initialAttendantPayProgressivePayout = 6, initialMachinePayProgressivePayout = 7;
                                    long currentCoinIn = 8, currentCoinOut = 9, currentJackpotHandpay = 10;
                                    long currentExternalBonusingMachinePayout = 11, currentExternalBonusingAttendantPayout = 12;
                                    long currentAttendantPayProgressivePayout = 13, currentMachinePayProgressivePayout = 14;
                                    byte[] nonTriggeredEventList = { 122, 0, 0, 0, 123, 0, 0, 0, 0 };
                                    int nonTriggeredEventsCount = 2;

                                    ConvertDateTimeToTimeStampArray(dateTime0, out initialTimeStamp);
                                    ConvertDateTimeToTimeStampArray(dateTime1, out currentTimeStamp);
                                    target.InEventTrigger(eventID, jackpotAmount, initialTimeStamp, ref initialTimeStampInString, currentTimeStamp, ref currentTimeStampInString,
                                        slotMastId, ref ocr, lastTriggeredEventId, initialCoinIn,
                                        initialCoinOut, initialJackpotHandpay, initialExternalBonusingMachinePayout,
                                        initialExternalBonusingAttendantPayout, initialAttendantPayProgressivePayout,
                                        initialMachinePayProgressivePayout, currentCoinIn, currentCoinOut,
                                        currentJackpotHandpay, currentExternalBonusingMachinePayout,
                                        currentExternalBonusingAttendantPayout, currentAttendantPayProgressivePayout,
                                        currentMachinePayProgressivePayout, nonTriggeredEventList,
                                        nonTriggeredEventsCount, ref functionInfo);
                                }
                                break;

                            case 20:
                                {
                                    funcName = "InTriggeredEventTrigger";
                                    int eventID = 123, triggeredEventID = 122, pointsEarned = 34;
                                    long jackpotAmount = 12345;
                                    DateTime dateTime0 = new DateTime(2000, 1, 2, 1, 23, 45, 678);
                                    DateTime dateTime1 = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] initialTimeStamp, currentTimeStamp;
                                    int slotMastId = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string initialTimeStampInString = string.Empty;
                                    string currentTimeStampInString = string.Empty;
                                    long initialCoinIn = 1, initialCoinOut = 2, initialJackpotHandpay = 3;
                                    long initialExternalBonusingMachinePayout = 4, initialExternalBonusingAttendantPayout = 5;
                                    long initialAttendantPayProgressivePayout = 6, initialMachinePayProgressivePayout = 7;
                                    long currentCoinIn = 8, currentCoinOut = 9, currentJackpotHandpay = 10;
                                    long currentExternalBonusingMachinePayout = 11, currentExternalBonusingAttendantPayout = 12;
                                    long currentAttendantPayProgressivePayout = 13, currentMachinePayProgressivePayout = 14;

                                    ConvertDateTimeToTimeStampArray(dateTime0, out initialTimeStamp);
                                    ConvertDateTimeToTimeStampArray(dateTime1, out currentTimeStamp);
                                    target.InTriggeredEventTrigger(eventID, triggeredEventID, pointsEarned, jackpotAmount,
                                        initialTimeStamp, ref initialTimeStampInString,  currentTimeStamp, ref currentTimeStampInString, slotMastId, ref ocr,
                                        initialCoinIn, initialCoinOut, initialJackpotHandpay,
                                        initialExternalBonusingMachinePayout, initialExternalBonusingAttendantPayout,
                                        initialAttendantPayProgressivePayout, initialMachinePayProgressivePayout,
                                        currentCoinIn, currentCoinOut, currentJackpotHandpay,
                                        currentExternalBonusingMachinePayout, currentExternalBonusingAttendantPayout,
                                        currentAttendantPayProgressivePayout, currentMachinePayProgressivePayout, ref functionInfo);
                                }
                                break;

                            case 21:
                                {
                                    funcName = "ReportEventMeterDeltas";
                                    int eventID = 123, triggeredEventID = 122;
                                    long jackpotAmount = 12345;
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] timeStamp;
                                    int slotMastId = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string timeStampInString = string.Empty;
                                    long splashdownPoints = 23;
                                    long coinInDelta = 1, coinOutDelta = 2;
                                    long jackpotHandpayDelta = 3, externalBonusingMachinePayoutDelta = 4;
                                    long externalBonusingAttendantPayoutDelta = 5, attendantPayProgressivePayoutDelta = 6;
                                    long machinePayProgressivePayoutDelta = 7, millisecondsPlayedDelta = 8000;
                                    int jackpotType = 0;
                                    double denomination = 0.25, par = 85.00;

                                    ConvertDateTimeToTimeStampArray(dateTime, out timeStamp);
                                    target.ReportEventMeterDeltas(eventID, triggeredEventID, jackpotAmount, timeStamp, ref timeStampInString, slotMastId,
                                        ref ocr, splashdownPoints, coinInDelta, coinOutDelta,
                                        jackpotHandpayDelta, externalBonusingMachinePayoutDelta,
                                        externalBonusingAttendantPayoutDelta, attendantPayProgressivePayoutDelta,
                                        machinePayProgressivePayoutDelta, millisecondsPlayedDelta, jackpotType,
                                        denomination, par, ref functionInfo);
                                }
                                break;

                            case 22:
                                {
                                    funcName = "ReportStatus";
                                    int slotMastId = 456;
                                    string status = "";
                                    functionInfo = string.Empty;
                                    target.ReportStatus(slotMastId, ref status, ref functionInfo);
                                }
                                break;

                            case 23:
                                {
                                    funcName = "ReportSentinelInformation";
                                    int slotMastId = 456;
                                    string osVersion = "1.0", exeVersion = "1.1", skinName = "Skin1";
                                    functionInfo = string.Empty;

                                    target.ReportSentinelInformation(slotMastId, ref osVersion, ref exeVersion, ref skinName, ref functionInfo);
                                }
                                break;

                            case 24:
                                {
                                    funcName = "GetThirdPartyBlob";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    // Out parameters.
                                    string sessionId;
                                    int vendorId, apiNotificationType, playerType;
                                    byte[] blob;
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetThirdPartyBlob(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, out sessionId, out vendorId,
                                        out apiNotificationType, out playerType, out blob, ref functionInfo);
                                }
                                break;

                            case 25:
                                {
                                    funcName = "SendBlobToThirdParty";
                                    string sessionId = "123";
                                    functionInfo = string.Empty;
                                    int slotmastId = 456;
                                    byte[] blob = new byte[5] { 0, 1, 2, 3, 4 };
                                    target.SendBlobToThirdParty(ref sessionId, slotmastId, blob, ref functionInfo);
                                }
                                break;

                            case 26:
                                {
                                    funcName = "GetEventTriggerNotification";
                                    DateTime dateTime = new DateTime(2001, 2, 3, 12, 34, 56, 789);
                                    byte[] lastTimeStamp;
                                    int id = 123, slotmastID = 456;
                                    string ocr = "1234567890";
                                    functionInfo = string.Empty;
                                    string lastTimeStampInString = string.Empty;
                                    // Out parameters.
                                    int eventId, lastTriggeredEventId;

                                    ConvertDateTimeToTimeStampArray(dateTime, out lastTimeStamp);
                                    target.GetEventTriggerNotification(lastTimeStamp, ref lastTimeStampInString, id, slotmastID, ref ocr, out eventId, 
                                        out lastTriggeredEventId, ref functionInfo);
                                }
                                break;

                            default:
                                done = true;
                                break;
                        }
                        if (!done)
                        {
                            Assert.Fail("Expected exception opening host, but exception did not occur, func({0})={1}", funcNum, funcName);
                        }
                    }
                    catch (System.ServiceModel.EndpointNotFoundException)
                    {
                        // The preferred exception for correct behavior.
                    }
                    catch (System.TimeoutException)
                    {
                        // This exception is also allowed to occur for correct behavior, but is not preferred.
                    }
                    catch (System.ServiceModel.CommunicationException)
                    {
                        // This exception is also allowed, but could indicate many different problems.
                    }
                    catch (Exception ex)
                    {
                        Assert.Fail("Unexpected exception setting/opening host, func({0})={1}: " + ex.Message, funcNum, funcName);
                    }
                    ++funcNum;
                } while (!done);
            }
            catch (Exception ex)
            {
                Assert.Fail("Unexpected exception during coverage test, func({0})={1}: " + ex.Message, funcNum, funcName);
            }
        }

    }
}
