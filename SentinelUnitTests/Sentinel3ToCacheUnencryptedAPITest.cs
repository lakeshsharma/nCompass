using SpeedMediaClient;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using System;
using System.ServiceModel;

namespace SentinelUnitTests
{
    
    
    /// <summary>
    ///This is a test class for Sentinel3ToCacheAPITest and is intended
    ///to contain all Sentinel3ToCacheAPITest Unit Tests
    ///</summary>
    [TestClass()]
	public class Sentinel3ToCacheUnencryptedAPITest
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
		public void SetBindingTypeUneTest()
		{
			try
			{
				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI();
				Sentinel3ToCacheUnencryptedAPI.BindingType bindingType = Sentinel3ToCacheUnencryptedAPI.BindingType.Http;
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
		public void SetBindingTypeUneTest2()
		{
			try
			{
				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI();
				target.SetBindingType(Sentinel3ToCacheUnencryptedAPI.BindingType.Https);
				target.SetBindingType(Sentinel3ToCacheUnencryptedAPI.BindingType.Http);
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
        public void SetBindingTypeUneTestPrivate()
        {
			try
            {
				const Sentinel3ToCacheUnencryptedAPI.BindingType bindingTypeExpected =
					Sentinel3ToCacheUnencryptedAPI.BindingType.Http;

				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI();
				var po = new PrivateObject(target);
                po.Invoke("SetBindingType", new object[] { bindingTypeExpected });

				// Get private value and convert to type used for private value.
				Sentinel3ToCacheUnencryptedAPI.BindingType bindingType =
					(Sentinel3ToCacheUnencryptedAPI.BindingType)po.GetField("_bindingType");
				if (bindingTypeExpected != bindingType)
				{
					Assert.Fail("_bindingType {0} != {1}", bindingType, bindingTypeExpected);
				}
            }
            catch (Exception ex)
            {
                Assert.Fail(ex.Message);
            }
        }

		/// <summary>
        ///A test for SetHost
        ///</summary>
        [TestMethod()]
        public void SetHostUneTest()
        {
            try
            {
				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI(); 
                string localHost = "127.0.0.1";
                target.SetHost(localHost);
            }
            catch (Exception ex)
            {
                Assert.Fail("Exception setting host: " + ex.Message);
            }
        }

		/// <summary>
		/// A test for open connection throwing an exception due to no endpoint listening.
		/// Try it again to cover code for closing comms.
		///</summary>
		[TestMethod()]
		public void SetHostOpenUneTest()
		{
			try
			{
				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI();
				target.SetHost("invalid.com");

				bool goodException = false;
				try
				{
					int dataSize = 10;
					byte[] dataBlock;

					// Verify that the next method call throws the following exception.
					//    "There was no endpoint listening at [...]"
					target.GetFlashDataBlock(ref dataSize, out dataBlock, 0, 0);
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
						int dataSize = 10;
						byte[] dataBlock;

						// Verify that the next method call throws the following exception.
						//    "There was no endpoint listening at [...]"
						target.GetFlashDataBlock(ref dataSize, out dataBlock, 0, 0);
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
				Assert.Fail("Unexpected exception setting/opening host: " + ex.Message);
			}
		}

		/// <summary>
		/// A test for open connection throwing an exception due to BindingType.
		///</summary>
		[TestMethod()]
		public void SetHostOpenUneTest2()
		{
			try
			{
				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI();
				target.SetBindingType(Sentinel3ToCacheUnencryptedAPI.BindingType.Https);
				target.SetHost("invalid.com");

				try
				{
					int dataSize = 10;
					byte[] dataBlock;

					// Verify that the next method call throws the following exception (for the BindingType).
					//    "Specified argument was out of the range of valid values."
					target.GetFlashDataBlock(ref dataSize, out dataBlock, 0, 0);
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
		public void SetHostUneTestPrivate()
		{
			try
			{
				const string localHostExpected = "127.0.0.1";
				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI();
				var po = new PrivateObject(target);
				po.Invoke("SetHost", new object[] { localHostExpected });

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
        public void CloseConnectionUneTest()
		{
			try
			{
				Sentinel3ToCacheUnencryptedAPI target = new Sentinel3ToCacheUnencryptedAPI();
				target.CloseConnection();
			}
			catch (Exception ex)
			{
				Assert.Fail("Exception closing connection: " + ex.Message);
			}
		}

		static readonly byte[] DataBlockFileId1 = new byte[13] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
		static readonly byte[] DataBlockFileId2 =
			new byte[23] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 };

		class CacheServiceStub: ISentinel3ToCacheUnencrypted
		{
			public bool GetFlashDataBlock(ref int dataSize, out byte[] dataBlock, int fileID, int blockNumber)
			{
				if (dataSize > 0 && blockNumber >= 0)
				{
					if (1 == fileID)
					{
						Int64 fileSize = DataBlockFileId1.Length;
						Int64 fileOffset = blockNumber * dataSize;
						Int64 retSize = Math.Min(dataSize, fileSize - fileOffset);

						dataSize = (int) retSize;
						dataBlock = new byte[dataSize];
						Array.Copy(DataBlockFileId1, fileOffset, dataBlock, 0, dataSize);
						return true;
					}
				}

				dataSize = 0;
				dataBlock = new byte[dataSize];
				return false;
			}

			public bool GetFlashDataBlocks(int fileID, int blockCount, byte[] blockList, out DataBlock[] blocks)
			{
				if (blockCount >= 0)
				{
					if (2 == fileID)
					{
						const Int64 BlockSize_Max = 10;
						Int64 fileSize = DataBlockFileId2.Length;
						Int64 blockNumber_Max = fileSize / BlockSize_Max;

						bool goodBlockNumbers = true;
						for (int blockIdx = 0; blockIdx < blockCount; ++blockIdx)
						{
							// Little-endian block numbers.
							int blockNumber =
								(blockList[blockIdx * 4 + 0] << 0) |
								(blockList[blockIdx * 4 + 1] << 8) |
								(blockList[blockIdx * 4 + 2] << 16) |
								(blockList[blockIdx * 4 + 3] << 24);
							if (blockNumber < 0 || blockNumber_Max < blockNumber)
							{
								goodBlockNumbers = false;
								break;
							}
						}

						if (goodBlockNumbers)
						{
							blocks = new DataBlock[blockCount];

							for (int blockIdx = 0; blockIdx < blockCount; ++blockIdx)
							{
								// Little-endian block numbers.
								int blockNumber =
									(blockList[blockIdx * 4 + 0] << 0) |
									(blockList[blockIdx * 4 + 1] << 8) |
									(blockList[blockIdx * 4 + 2] << 16) |
									(blockList[blockIdx * 4 + 3] << 24);
								Int64 fileOffset = blockNumber * BlockSize_Max;

								blocks[blockIdx] = new DataBlock();
								blocks[blockIdx].BlockNumber = blockNumber;
								blocks[blockIdx].Size = (int) Math.Min(BlockSize_Max, fileSize - fileOffset);
								blocks[blockIdx].Block = new byte[blocks[blockIdx].Size];
								Array.Copy(DataBlockFileId2, fileOffset, blocks[blockIdx].Block, 0, blocks[blockIdx].Size);
							}
							return true;
						}
					}
				}

				blocks = new DataBlock[blockCount];
				for (int blockIdx = 0; blockIdx < blockCount; ++blockIdx)
				{
					blocks[blockIdx] = new DataBlock();
					blocks[blockIdx].BlockNumber = 0;
					blocks[blockIdx].Size = 0;
					blocks[blockIdx].Block = null;
				}
				return false;
			}
		}

		/// <summary>
		///A test for GetFlashDataBlock
		///</summary>
		/// Note (2010-03-24): Attempting to use an Moq Mock "Object" for the cacheService generates the 
		/// the following exception at runtime with Moq 3.0.308.2, Moq 3.1.416.3, and Moq 4.0.812.4.
		///     System.ArgumentException:
		///     A matching constructor for the given arguments was not found on the mocked type.
		///     ---> System.MissingMethodException: Constructor on type 'Castle.Proxies.ChannelFactory [...] not found.
		/// This appears to be a known Moq issue with a Medium Priority.
		/// Issue 203:  unable to mock a class with a constructor marked as internal  
		/// http://code.google.com/p/moq/issues/detail?id=203
		[TestMethod()]
		public void GetFlashDataBlockUneTest()
		{
			try
			{
				const int dataSize_Max = 10;
				int dataSize;
				byte[] dataBlock;
				int blockNumber;
				bool retVal;
				var targetMock = new Mock<Sentinel3ToCacheUnencryptedAPI>();
				targetMock.Setup(obj => obj.GetNewCacheService(It.IsAny<BasicHttpBinding>(), It.IsAny<EndpointAddress>()))
					.Returns(new CacheServiceStub());

				targetMock.Object.SetHost("localhost");
				targetMock.Object.SetBindingType(Sentinel3ToCacheUnencryptedAPI.BindingType.Http);

				// Check that file does NOT exist.
				dataSize = dataSize_Max;
				blockNumber = 0;
				retVal = targetMock.Object.GetFlashDataBlock(ref dataSize, out dataBlock, -22, blockNumber);
				if (retVal)
				{
					Assert.Fail("No File: retVal {0} != {1}", retVal, false);
				}
				else if (null == dataBlock)
				{
					Assert.Fail("No File: dataBlock == null");
				}
				else if (0 != dataBlock.Length)
				{
					Assert.Fail("No File: dataBlock.Length {0} != {1}", dataBlock.Length, 0);
				}

				// Check first block for fileID==1.
				dataSize = dataSize_Max;
				blockNumber = 0;
				retVal = targetMock.Object.GetFlashDataBlock(ref dataSize, out dataBlock, 1, blockNumber);
				if (!retVal)
				{
					Assert.Fail("file 1 block 0: retVal {0} != {1}", retVal, true);
				}
				else if (null == dataBlock)
				{
					Assert.Fail("file 1 block 0: dataBlock == null");
				}
				else if (dataSize_Max != dataBlock.Length)
				{
					Assert.Fail("file 1 block 0: dataBlock.Length {0} != {1}", dataBlock.Length, dataSize_Max);
				}
				else
				{
					for (int idx = 0; idx < dataBlock.Length; ++idx)
					{
						if (dataBlock[idx] != DataBlockFileId1[dataSize_Max * blockNumber + idx])
						{
							Assert.Fail("file 1 block 0: dataBlock[{0}] != {1}",
								dataBlock[idx], DataBlockFileId1[dataSize_Max * blockNumber + idx]);
						}
					}
				}

				// Check last block for fileID==1.
				dataSize = dataSize_Max;
				blockNumber = 1;
				retVal = targetMock.Object.GetFlashDataBlock(ref dataSize, out dataBlock, 1, blockNumber);
				if (!retVal)
				{
					Assert.Fail("file 1 block 0: retVal {0} != {1}", retVal, true);
				}
				else if (null == dataBlock)
				{
					Assert.Fail("file 1 block 0: dataBlock == null");
				}
				else if (DataBlockFileId1.Length % dataSize_Max != dataBlock.Length)
				{
					Assert.Fail("file 1 block 0: dataBlock.Length {0} != {1}", dataBlock.Length, DataBlockFileId1.Length % dataSize_Max);
				}
				else
				{
					for (int idx = 0; idx < dataBlock.Length; ++idx)
					{
						if (dataBlock[idx] != DataBlockFileId1[dataSize_Max * blockNumber + idx])
						{
							Assert.Fail("file 1 block 0: dataBlock[{0}] != {1}",
								dataBlock[idx], DataBlockFileId1[dataSize_Max * blockNumber + idx]);
						}
					}
				}
			}
			catch (Exception ex)
			{
				Assert.Fail(ex.Message);
			}
		}

		/// <summary>
		///A test for GetFlashDataBlocks
		///</summary>
		[TestMethod()]
		public void GetFlashDataBlocksUneTest()
		{
			try
			{
				DataBlockStruct[] blocks;
				bool retVal;

				var targetMock = new Mock<Sentinel3ToCacheUnencryptedAPI>();
				targetMock.Setup(obj => obj.GetNewCacheService(It.IsAny<BasicHttpBinding>(), It.IsAny<EndpointAddress>()))
					.Returns(new CacheServiceStub());

				targetMock.Object.SetHost("localhost");
				targetMock.Object.SetBindingType(Sentinel3ToCacheUnencryptedAPI.BindingType.Http);

				const int blockCount = 3;
				byte[] blockList = new byte[blockCount * 4] {
					0, 0, 0, 0, // Little-endian blockNumber 0.
					1, 0, 0, 0, // Little-endian blockNumber 1.
					2, 0, 0, 0}; // Little-endian blockNumber 2.


				// Check that file does NOT exist.
				retVal = targetMock.Object.GetFlashDataBlocks(-33, blockCount, blockList, out blocks);
				if (retVal)
				{
					Assert.Fail("No File: retVal {0} != {1}", retVal, false);
				}
				else if (null == blocks)
				{
					Assert.Fail("No File: blocks == null");
				}
				else if (blockCount != blocks.Length)
				{
					Assert.Fail("No File: blocks.Length {0} != {1}", blocks.Length, blockCount);
				}

				// Check that file does exist.
				retVal = targetMock.Object.GetFlashDataBlocks(2, blockCount, blockList, out blocks);
				if (!retVal)
				{
					Assert.Fail("file 2: retVal {0} != {1}", retVal, true);
				}
				else if (null == blocks)
				{
					Assert.Fail("file 2: blocks == null");
				}
				else if (blockCount != blocks.Length)
				{
					Assert.Fail("file 2: blocks.Length {0} != {1}", blocks.Length, blockCount);
				}
				else
				{
					int dataIdx = 0;
					for (int blockIdx = 0; blockIdx < blocks.Length; ++blockIdx)
					{
						if (blocks[blockIdx].blockNumber != blockIdx)
						{
							Assert.Fail("file 2: blocks[{0}].BlockNumber {1} != {2}", blockIdx, blocks[blockIdx].blockNumber, blockIdx);
						}

						for (int idx = 0; idx < blocks[blockIdx].size; ++idx)
						{
							if (blocks[blockIdx].block[idx] != DataBlockFileId2[dataIdx])
							{
								Assert.Fail("file 2: blocks[{0}].block[{1}] {2} != {3}",
									blockIdx, idx, blocks[blockIdx].block[idx], DataBlockFileId2[dataIdx]);
							}
							++dataIdx;
						}
					}

					if (DataBlockFileId2.Length != dataIdx)
					{
						Assert.Fail("file 2: Sum of block[].Size {0} != {1}", dataIdx, DataBlockFileId2.Length);
					}
				}
			}
			catch (Exception ex)
			{
				Assert.Fail(ex.Message);
			}
		}

	}
}
