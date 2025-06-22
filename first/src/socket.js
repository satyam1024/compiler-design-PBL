import { io } from "socket.io-client";

const socket = io("http://localhost:9900");

export default socket;
