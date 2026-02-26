namespace Client_2
{
    internal class Protocol
    {
        public const int REQUEST_HEADER_SIZE_BYTES = 4;
        public const int COMMAND_BYTES = 1;
        public const int FILENAME_LENGTH_BYTES = 2;
        public const int FILE_SIZE_BYTES = 4;

        public const int STATUS_BYTES = 1;
        public const int PAYLOAD_LENGTH_BYTES = 4;

        public const int CHUNK_SIZE = 1024;

        public enum Command : byte
        {
            Get,
	        List,
	        Put,
	        Delete,
	        Info,
	        Unknown
        };

        public enum Status : byte
        {
            SUCCESS,
	        FILE_NOT_FOUND,
	        INVALID_REQUEST,
	        INTERNAL_ERROR
        };
    }
}
