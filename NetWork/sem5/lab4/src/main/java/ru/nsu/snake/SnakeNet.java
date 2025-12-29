package ru.nsu.snake;

import ru.nsu.snake.model.GameField;
import ru.nsu.snake.model.GameLogic;
import ru.nsu.snake.model.Snake;
import ru.nsu.snake.ui.ServerInfo;

import java.io.IOException;
import java.net.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;

public class SnakeNet {
    public static final String MULTICAST_ADDRESS = "239.192.0.4";
    public static final int MULTICAST_PORT = 9192;
    private final Observer observer;
    private final ConcurrentHashMap<Integer, Long> lastMsgSeqReceived = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<Integer, ConcurrentHashMap<Long, MessageInfo>> sentMessages = new ConcurrentHashMap<>();
    private final long announcementDelayMS = 1000;
    private final long pingDelayMS = 100;
    private final long resendTime;
    private final long waitTime;
    private final Object lockGameField = new Object();
    private GameField gameField;
    private volatile SnakesProto.GameMessage.StateMsg lastStateMsg;
    private volatile int playerId = -1;
    private volatile int deputyId = -1;
    private volatile int masterId = -1;
    private volatile InetAddress serverAddress;
    private volatile int serverPort;
    private final AtomicLong msgSeq = new AtomicLong(0);
    private int stateOrder = 0;
    private final Object lockSnakeGame = new Object();
    private GameLogic snakeGame = null;
    private final ConcurrentHashMap<Integer, SnakesProto.GamePlayer> players = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<InetSocketAddress, Integer> addressToPlayerId = new ConcurrentHashMap<>();
    private volatile int currentMaxId = 0;
    private final int maxPlayerCount = 5;
    private DatagramSocket socket;
    private final ServerInfo serverInfo;
    private Thread messageReceiverLoop;
    private Thread announcementSendThread;
    private Thread gameLoop;
    private Thread messageResenderThread;
    private Thread pingSender;
    private volatile SnakesProto.NodeRole nodeRole;
    private volatile InetSocketAddress currentMasterAddr = null;
    private volatile int currentMasterId = -1;
    private volatile long lastStateOrder = -1;
    private static final boolean LOG_FAILOVER = true;

    public SnakeNet(ServerInfo serverInfo, Observer observer) throws IOException {
        this.serverInfo = serverInfo;
        this.observer = observer;

            this.resendTime = serverInfo.stateDelayMsProperty().get() / 10;
            this.waitTime = resendTime * 8;
            synchronized (lockGameField) {
                this.gameField = new GameField(serverInfo);
            }
        String serverAddress = serverInfo.serverIPProperty().get();
        System.out.println("[SnakeNet] Address: " + serverAddress + ":" + serverInfo.serverPortProperty().get());
        messageReceiverLoop = new Thread(this::receiveMessageLoop);
        messageResenderThread = new Thread(() -> {
            while (!Thread.interrupted()) {
                try {
                    long currentTime = System.currentTimeMillis();
                    for (ConcurrentHashMap<Long, MessageInfo> playerMessages : sentMessages.values()) {
                        for (MessageInfo message : playerMessages.values()) {
                            System.out.println("Resender: curr time = " + currentTime + "   message time = " + message.getTimestamp());
                            boolean shouldResend = (currentTime - message.getTimestamp()) > resendTime;
                            if (shouldResend) {
                                message.setAttemptCount(message.getAttemptCount() + 1);
                                if (currentTime - message.getTimestamp() > waitTime) {
                                    System.err.println("Player " + message.getMessage().getReceiverId() + " doesn't answer...");
                                    handlePlayerDisconnection(message.getMessage().getReceiverId());
                                    break;
                                }
                                if (message.isToMaster()) {
                                    message.setPort(this.serverPort);
                                    message.setAddress(this.serverAddress);
                                }
                                sendGameMessage(message.getMessage(), message.getAddress(), message.getPort());
                            }
                        }

                    }

                    Thread.sleep(resendTime);
                } catch (InterruptedException e) {
                    System.err.println("Resend message error!");
                    break;
                }
            }
        });

        messageReceiverLoop.start();
        messageResenderThread.start();
    }

    public void startAsClient(String playerName, InetAddress serverAddress, int serverPort, SnakesProto.NodeRole role) throws IOException {
        this.nodeRole = role;
        this.serverAddress = serverAddress;
        this.serverPort = serverPort;
        this.socket = new DatagramSocket();

        sendJoinRequest(playerName, serverAddress, serverPort, role);

        pingSender = new Thread(() -> {
            while (!pingSender.isInterrupted()) {
                try {
                    Thread.sleep(pingDelayMS);
                    if (masterId == -1) continue;
                    if (sentMessages.get(masterId).isEmpty()) {
                        SnakesProto.GameMessage pingMsg = createPingMessage();
                        sendGameMessage(pingMsg, serverAddress, serverPort);
                    }
                } catch (InterruptedException e) {
                    System.err.println("Ping sender error...");
                    break;
                } catch (Exception e) {

                }
            }
        });
        pingSender.start();
    }
    public void startAsServer(String playerName, InetAddress address, int port) throws IOException {
        this.nodeRole = SnakesProto.NodeRole.MASTER;
        synchronized (lockGameField) {
            synchronized (lockSnakeGame) {
                this.snakeGame = new GameLogic(gameField, lockGameField);
            }
        }
        this.serverAddress = address;
        this.serverPort = port;
        this.socket = new DatagramSocket(serverPort, serverAddress);
        //this.socket.setOption(StandardSocketOptions.IP_MULTICAST_TTL, 1);
        serverPort = socket.getLocalPort();
        serverInfo.setServerPort(serverPort);

        int playerId = addNewPlayer(playerName, address, serverPort, SnakesProto.NodeRole.MASTER);
        this.playerId = playerId;
        masterId = playerId;
        try {
            ArrayList<SnakesProto.GameState.Coord> initialPosition = new ArrayList<>();
            SnakesProto.Direction headDirection;
            synchronized (lockSnakeGame) {
                headDirection = snakeGame.getGameField().findValidSnakePosition(initialPosition);
            }
            System.out.println("NET headDirection = " + headDirection.name());
            Snake newSnake = new Snake(initialPosition, playerId);

            newSnake.setHeadDirection(headDirection);
            newSnake.setNextDirection(headDirection);

            synchronized (lockSnakeGame) {
                snakeGame.addSnake(newSnake);
                System.out.println("Add new snake, position " + initialPosition.get(0).getX() + " " + initialPosition.get(0).getY() + " snakes count " + snakeGame.getGameField().getSnakes().size());
            }

            startServerThreads();
        } catch (Exception e) {
            System.err.println("[Server] Player " + playerName + " cannot join the game");
            sendError("Cannot join the game: no space", address, port);
        }
    }
    private void startServerThreads() {

        if (pingSender != null) pingSender.interrupt();

        announcementSendThread = new Thread(() -> {
            while (!announcementSendThread.isInterrupted()) {
                try {
                    SnakesProto.GameMessage announcement = createAnnouncementMessage();
                    sendGameMessage(announcement, InetAddress.getByName(MULTICAST_ADDRESS), MULTICAST_PORT);
                    Thread.sleep(announcementDelayMS);
                } catch (InterruptedException | IOException e) {
                    System.err.println("[Server] Announcement send error!");
                    break;
                }
            }
        });

        gameLoop = new Thread(() -> {
            while (!gameLoop.isInterrupted()) {
                try {
                    int delay;
                    synchronized (lockSnakeGame) {
                        delay = snakeGame.getGameField().getDelayMS();
                    }
                        Thread.sleep(delay);
                    synchronized (lockSnakeGame) {
                        snakeGame.update();
                    }
                    updatePlayersScore();
                    sendStateForAll();
                } catch (InterruptedException | IOException e) {
                    System.err.println("[Server] Game loop destroyed...");
                    break;
                }
            }
        });

        announcementSendThread.start();
        gameLoop.start();
    }
    private SnakesProto.GameMessage createAnnouncementMessage() {
        GameField field;
        synchronized (lockSnakeGame) {
            field = snakeGame.getGameField();
        }
        String serverName = serverInfo.serverNameProperty().get();
        return SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msgSeq.incrementAndGet())
                .setAnnouncement(SnakesProto.GameMessage.AnnouncementMsg.newBuilder()
                        .addGames(SnakesProto.GameAnnouncement.newBuilder()
                                .setPlayers(SnakesProto.GamePlayers.newBuilder()
                                        .addAllPlayers(players.values())
                                        .build())
                                .setConfig(SnakesProto.GameConfig.newBuilder()
                                        .setWidth(field.getWidth())
                                        .setHeight(field.getHeight())
                                        .setFoodStatic(field.getFoodStatic())
                                        .setStateDelayMs(field.getDelayMS())
                                        .build())
                                .setCanJoin(players.size() < maxPlayerCount)
                                .setGameName(serverName)
                                .build())
                        .build())
                .build();
    }
    private SnakesProto.GameMessage createPingMessage() {
        return SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msgSeq.incrementAndGet())
                .setPing(SnakesProto.GameMessage.PingMsg.newBuilder().build())
                .build();
    }
    public void updatePlayersScore() {
        List<Snake> snakes;
        synchronized (lockSnakeGame) {
            snakes = snakeGame.getGameField().getSnakes();
        }
        for (Snake snake : snakes) {
            int playerId = snake.getPlayerID();
            SnakesProto.GamePlayer player = players.get(playerId);
            if (player != null) {
                SnakesProto.GamePlayer updatedPlayer = SnakesProto.GamePlayer.newBuilder()
                        .mergeFrom(player)
                        .setScore(snake.getScore())
                        .build();
                players.put(playerId, updatedPlayer);
            }
        }
    }
    private void removePlayer(int playerId) {
        if (!players.containsKey(playerId)) {
            System.err.println("Player ID " + playerId + " not found in the current players list.");
            return;
        }

        players.remove(playerId);
        addressToPlayerId.values().removeIf(id -> id == playerId);

        sentMessages.get(playerId).clear();

        System.out.println("Player " + playerId + " successfully removed!");
    }
    private void disconnectPlayerToZombie(int playerId) {
        if (!players.containsKey(playerId)) {
            System.err.println("Player ID " + playerId + " not found in the current players list.");
            return;
        }

        // 1) игрок больше не управляет
        SnakesProto.GamePlayer p = players.get(playerId);
        players.put(playerId, SnakesProto.GamePlayer.newBuilder(p)
                .setRole(SnakesProto.NodeRole.VIEWER)
                .build());

        // 2) адрес/канал связи удаляем (узел считается "выпал")
        addressToPlayerId.values().removeIf(id -> id == playerId);

        // 3) чистим очередь неподтверждённых к нему (если есть)
        if (sentMessages.containsKey(playerId) && sentMessages.get(playerId) != null) {
            sentMessages.get(playerId).clear();
        }

        // 4) переводим его змейку в ZOMBIE (если змея есть на поле)
        synchronized (lockGameField) {
            for (ru.nsu.snake.model.Snake s : gameField.getSnakes()) {
                if (s.getPlayerID() == playerId) {
                    s.setState(SnakesProto.GameState.Snake.SnakeState.ZOMBIE);
                    break;
                }
            }
        }

        System.out.println("Player " + playerId + " became VIEWER, snake became ZOMBIE.");
    }


    private void receiveMessageLoop() {
        while (!Thread.currentThread().isInterrupted()) {
            if (socket == null) {
                try { Thread.sleep(10); } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
                continue;
            }

            byte[] buffer = new byte[4096];
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

            try {
                socket.receive(packet);
            } catch (IOException e) {
                System.err.println("Message receive error (socket.receive): " + e.getMessage());
                e.printStackTrace();
                break; // сокет реально отвалился
            }

            // Всё, что дальше — может упасть чем угодно. Это НЕ должно убивать receive loop.
            try {
                byte[] data = Arrays.copyOfRange(packet.getData(), packet.getOffset(), packet.getLength());

                InetAddress address = packet.getAddress();
                int port = packet.getPort();

                SnakesProto.GameMessage message = SnakesProto.GameMessage.parseFrom(data);

                if (players.get(message.getSenderId()) != null) {
                    System.out.println("listened " + message.getTypeCase()
                            + "(rec: " + message.getReceiverId()
                            + ", sen: " + message.getSenderId()
                            + ", seq: " + message.getMsgSeq()
                            + ") from " + address + ":" + port
                            + "(" + players.get(message.getSenderId()).getRole() + ")");
                } else {
                    System.out.println("listened " + message.getTypeCase()
                            + "(rec: " + message.getReceiverId()
                            + ", sen: " + message.getSenderId()
                            + ", seq: " + message.getMsgSeq()
                            + ") from " + address + ":" + port);
                }

                // ACK логика
                if (message.getTypeCase() != SnakesProto.GameMessage.TypeCase.ANNOUNCEMENT
                        && message.getTypeCase() != SnakesProto.GameMessage.TypeCase.ACK
                        && message.getTypeCase() != SnakesProto.GameMessage.TypeCase.JOIN) {
                    sendAck(message.getMsgSeq(), address, port);
                } else if (message.getTypeCase() == SnakesProto.GameMessage.TypeCase.ACK) {
                    handleAck(message, address, port);
                    continue;
                }

                int playerId = message.getSenderId();
                long playerMsgSeq = message.getMsgSeq();

                if (playerMsgSeq > lastMsgSeqReceived.getOrDefault(playerId, -1L)) {
                    if (playerId != -1) lastMsgSeqReceived.put(playerId, playerMsgSeq);

                    switch (message.getTypeCase()) {
                        case PING -> {
                            // можно обновлять lastSeen и т.п.
                        }
                        case STEER -> handleSteer(message, address, port);
                        case JOIN -> handleJoin(message, address, port);
                        case ANNOUNCEMENT -> {
                        }
                        case STATE -> handleState(message, address, port);
                        case ERROR -> handleError(message, address, port);
                        case ROLE_CHANGE -> handleRoleChange(message, address, port);
                        default -> {
                            System.err.println("Unknown message type (" + message.getTypeCase() + ") from " + address + ":" + port);
                            sendError("Unknown message type", address, port);
                            // НЕ return; иначе один плохой пакет убьёт receive loop
                        }
                    }
                }

                if (nodeRole == SnakesProto.NodeRole.MASTER && deputyId == -1) {
                    selectNewDeputy();
                }

            } catch (Throwable t) {
                System.err.println("[RECEIVER] crash while handling packet from "
                        + packet.getAddress() + ":" + packet.getPort());
                t.printStackTrace();
                // продолжаем цикл
            }
        }
    }

    public void sendJoinRequest(String playerName, InetAddress address, int port, SnakesProto.NodeRole role) {
        String serverName = serverInfo.serverNameProperty().get();

        SnakesProto.GameMessage joinMessage = SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msgSeq.incrementAndGet())
                .setJoin(SnakesProto.GameMessage.JoinMsg.newBuilder()
                        .setPlayerName(playerName)
                        .setGameName(serverName)
                        .setRequestedRole(role)
                        .build())
                .build();

        sendGameMessage(joinMessage, address, port);
    }

    private void handleSteer(SnakesProto.GameMessage message, InetAddress address, int port) {
        SnakesProto.GameMessage.SteerMsg steer = message.getSteer();
        int playerId = getPlayerIdByAddress(address, port);

        if (playerId != -1) {
            SnakesProto.Direction direction = steer.getDirection();
            synchronized (lockSnakeGame) {
                snakeGame.updateDirection(playerId, direction);
            }
        } else {
            System.err.println("[Server] Unknown player from " + address + ":" + port);
        }
    }
    private void handleJoin(SnakesProto.GameMessage message, InetAddress address, int port) throws IOException {
        SnakesProto.GameMessage.JoinMsg join = message.getJoin();
        SnakesProto.NodeRole requestedRole = join.getRequestedRole();

        boolean canJoinAsPlayer = requestedRole != SnakesProto.NodeRole.VIEWER && players.size() < maxPlayerCount;
        boolean canJoinAsViewer = requestedRole == SnakesProto.NodeRole.VIEWER;

        if (!canJoinAsPlayer && !canJoinAsViewer) {
            sendError("Cannot join: game full", address, port);
            return;
        }

        int playerId = addNewPlayer(join.getPlayerName(), address, port, requestedRole);
        if (playerId == -1) {
            sendError("Role conflict", address, port);
            return;
        }

        if (requestedRole != SnakesProto.NodeRole.VIEWER) {
            ArrayList<SnakesProto.GameState.Coord> initialPosition = new ArrayList<>();
            SnakesProto.Direction headDirection;
            synchronized (lockSnakeGame) {
                headDirection = snakeGame.getGameField().findValidSnakePosition(initialPosition);
            }
            Snake newSnake = new Snake(initialPosition, playerId);
            newSnake.setHeadDirection(headDirection);
            newSnake.setNextDirection(headDirection);
            synchronized (lockSnakeGame) {
                snakeGame.addSnake(newSnake);
            }
        }

        System.err.println("Player(" + playerId + ") joined as " + requestedRole);
        sendAck(message.getMsgSeq(), address, port);
    }
    public void sendSteer(SnakesProto.Direction direction) {
        SnakesProto.GameMessage steerMessage = SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msgSeq.incrementAndGet())
                .setSteer(SnakesProto.GameMessage.SteerMsg.newBuilder()
                        .setDirection(direction)
                        .build())
                .build();

        System.out.println("sendSteer");
        sendGameMessage(steerMessage, serverAddress, serverPort);
    }
    public void sendStateForAll() throws IOException {
        SnakesProto.GameMessage stateMessage = createStateMessage();
        for (SnakesProto.GamePlayer player : new ArrayList<>(players.values())) {
            sendGameMessage(stateMessage, InetAddress.getByName(player.getIpAddress()), player.getPort());
        }
    }
    private SnakesProto.GameMessage createStateMessage() {
        SnakesProto.GameState.Builder gameStateBuilder = SnakesProto.GameState.newBuilder()
                .setStateOrder(++stateOrder);

        synchronized (lockSnakeGame) {
            gameStateBuilder.addAllFoods(snakeGame.getGameField().getFoods());
        }

        List<Snake> snakes;
        int width;
        int height;
        synchronized (lockSnakeGame) {
            snakes = snakeGame.getGameField().getSnakes();
            width = snakeGame.getGameField().getWidth();
            height = snakeGame.getGameField().getHeight();
        }
        for (Snake snake : snakes) {
            gameStateBuilder.addSnakes(Snake.generateSnakeProto(snake, height, width));
        }

        gameStateBuilder.setPlayers(SnakesProto.GamePlayers.newBuilder().addAllPlayers(players.values()).build());

        return SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msgSeq.incrementAndGet())
                .setState(SnakesProto.GameMessage.StateMsg.newBuilder().setState(gameStateBuilder.build()).build())
                .build();
    }
    private void handleState(SnakesProto.GameMessage message, InetAddress address, int port) {
        SnakesProto.GameMessage.StateMsg stateMsg = message.getState();
        lastStateMsg = stateMsg;

        InetSocketAddress src = new InetSocketAddress(address, port);
        long order = stateMsg.getState().getStateOrder();

        try {
            // 1) Узнаём MASTER из содержимого State (важно сделать ДО ignore по адресу!)
            InetSocketAddress masterAddrFromState = null;
            int masterIdFromState = -1;

            for (SnakesProto.GamePlayer p : stateMsg.getState().getPlayers().getPlayersList()) {
                if (p.getId() != -1 && p.getRole() == SnakesProto.NodeRole.MASTER) {
                    try {
                        masterAddrFromState = new InetSocketAddress(
                                InetAddress.getByName(p.getIpAddress()),
                                p.getPort()
                        );
                        masterIdFromState = p.getId();
                    } catch (UnknownHostException e) {
                        System.err.println("Bad MASTER host in state: " + p.getIpAddress() + ":" + p.getPort());
                    }
                    break;
                }
            }

            // 2) Если мастер изменился — переключаемся
            if (masterAddrFromState != null) {
                boolean changed =
                        (currentMasterAddr == null) ||
                                (currentMasterId != masterIdFromState) ||
                                (!currentMasterAddr.equals(masterAddrFromState));

                if (changed) {
                    onMasterChanged(masterAddrFromState, masterIdFromState);
                }
            }

            // 3) Теперь можно фильтровать: STATE принимаем только от текущего мастера
            if (currentMasterAddr != null && !currentMasterAddr.equals(src)) {
                System.out.println("[STATE] ignore from " + src + " (current master " + currentMasterAddr + ")");
                return;
            }

            // 4) Фильтр по state_order
            if (order <= lastStateOrder) return;
            lastStateOrder = order;

            // 5) Синхронизируем serverAddress/serverPort (чтобы steer/ping шли туда)
            if (currentMasterAddr != null) {
                serverAddress = currentMasterAddr.getAddress();
                serverPort = currentMasterAddr.getPort();
                masterId = currentMasterId;
            }

            // 6) Обновляем игроков
            players.clear();
            addressToPlayerId.clear();

            for (SnakesProto.GamePlayer player : stateMsg.getState().getPlayers().getPlayersList()) {
                if (player.getId() == -1) continue;

                if (player.getRole() == SnakesProto.NodeRole.DEPUTY) {
                    deputyId = player.getId();
                }

                players.put(player.getId(), player);

                try {
                    InetSocketAddress pAddr = new InetSocketAddress(
                            InetAddress.getByName(player.getIpAddress()),
                            player.getPort()
                    );
                    addressToPlayerId.put(pAddr, player.getId());
                } catch (UnknownHostException ignored) { }

                sentMessages.computeIfAbsent(player.getId(), k -> new ConcurrentHashMap<>());
            }

        } catch (Throwable t) {
            System.err.println("[handleState] crash, from " + src + " order=" + order);
            t.printStackTrace();
            return;
        }

        // 7) UI update через FX thread
        try {
            javafx.application.Platform.runLater(() -> {
                try {
                    observer.update(stateMsg, address, port);
                } catch (Throwable t) {
                    System.err.println("[observer.update] crashed on FX thread");
                    t.printStackTrace();
                }
            });
        } catch (Throwable t) {
            observer.update(stateMsg, address, port);
        }
    }

    private void handleAck(SnakesProto.GameMessage message, InetAddress address, int port) {
        if (this.playerId == -1) {
            this.playerId = message.getReceiverId();
            sentMessages.put(message.getSenderId(), new ConcurrentHashMap<>());
            addressToPlayerId.put(new InetSocketAddress(address, port), message.getSenderId());
        }
        try {
            sentMessages.get(message.getSenderId()).remove(message.getMsgSeq());
        } catch (Exception e) {
        }
    }
    private void handleError(SnakesProto.GameMessage message, InetAddress address, int port) {
        SnakesProto.GameMessage.ErrorMsg error = message.getError();
        System.err.println("[Server] Error: " + error.getErrorMessage() + " from " + address.toString() + port);
    }
    private void handleRoleChange(SnakesProto.GameMessage message, InetAddress address, int port) throws UnknownHostException {
        SnakesProto.GameMessage.RoleChangeMsg roleChangeMsg = message.getRoleChange();
        SnakesProto.NodeRole receiverRole = roleChangeMsg.getReceiverRole();
        SnakesProto.NodeRole senderRole = roleChangeMsg.getSenderRole();

        if (nodeRole == receiverRole && senderRole == SnakesProto.NodeRole.MASTER) {
            masterId = message.getSenderId();
            serverPort = port;
            serverAddress = address;
            System.out.println("new master port server " + serverPort + " " + serverAddress);
            players.put(masterId, SnakesProto.GamePlayer.newBuilder(players.get(masterId)).setRole(SnakesProto.NodeRole.MASTER).build());
            return;
        }

        // мастер отправил, что я  новый депути
        if (receiverRole == SnakesProto.NodeRole.DEPUTY) {
            deputyId = message.getReceiverId();
            if (playerId == deputyId) {
                nodeRole = SnakesProto.NodeRole.DEPUTY;
            }
            if (players.get(deputyId) == null) {
                return;
            }
            players.put(deputyId, SnakesProto.GamePlayer.newBuilder(players.get(deputyId)).setRole(SnakesProto.NodeRole.DEPUTY).build());

            if (masterId != message.getSenderId()) {
                masterId = message.getSenderId();
                serverPort = port;
                serverAddress = address;
            }
            return;
        }

        // мы депути и матер сказал его заменить
        if (receiverRole == SnakesProto.NodeRole.MASTER) {
            /*if (nodeRole != SnakesProto.NodeRole.DEPUTY) {
                System.out.println("STRANGE MESSAGE CHANGE ROLE: new receiverRole MASTER, but player isn't DEPUTY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                return;
            }*/

            pingSender.interrupt();

            currentMaxId = players.values().stream()
                    .map(SnakesProto.GamePlayer::getId)
                    .max(Integer::compare)
                    .get();

            SnakesProto.GamePlayer oldPlayer = players.get(message.getReceiverId());
            SnakesProto.GamePlayer player = SnakesProto.GamePlayer.newBuilder(oldPlayer).setRole(SnakesProto.NodeRole.MASTER).build();
            players.put(player.getId(), player);

            removePlayer(masterId);

            masterId = player.getId();
            nodeRole = SnakesProto.NodeRole.MASTER;
            deputyId = -1;

            this.serverAddress = InetAddress.getByName(player.getIpAddress());
            this.serverPort = player.getPort();

            synchronized (lockGameField) {
                GameLogic.editGameFieldFromState(gameField, lastStateMsg);
                synchronized (lockSnakeGame) {
                    snakeGame = new GameLogic(gameField, lockGameField);
                }
            }
            startServerThreads();
        }
    }
    private void sendError(String errorMessage, InetAddress address, int port) throws IOException {
        SnakesProto.GameMessage error = SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msgSeq.incrementAndGet())
                .setError(SnakesProto.GameMessage.ErrorMsg.newBuilder().setErrorMessage(errorMessage).build())
                .build();

        sendGameMessage(error, address, port);
    }
    private void sendAck(long msg_seq, InetAddress address, int port) throws IOException {
        SnakesProto.GameMessage ack = SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msg_seq)
                .setAck(SnakesProto.GameMessage.AckMsg.newBuilder().build())
                .build();

        sendGameMessage(ack, address, port);
    }
    public void sendRoleChange(SnakesProto.NodeRole receiverRole, SnakesProto.NodeRole senderRole, int receiverId) throws IOException {
        SnakesProto.GameMessage roleChangeMessage = SnakesProto.GameMessage.newBuilder()
                .setMsgSeq(msgSeq.incrementAndGet())
                .setRoleChange(SnakesProto.GameMessage.RoleChangeMsg.newBuilder()
                        .setReceiverRole(receiverRole)
                        .setSenderRole(senderRole)
                        .build())
                .build();

        SnakesProto.GamePlayer receiver = players.get(receiverId);
        if (receiver != null) {
            sendGameMessage(roleChangeMessage, InetAddress.getByName(receiver.getIpAddress()), receiver.getPort());
        }
    }
    private void selectNewDeputy() {
        for (Map.Entry<Integer, SnakesProto.GamePlayer> entry : players.entrySet()) {
            SnakesProto.GamePlayer player = entry.getValue();
            if (player.getRole() == SnakesProto.NodeRole.NORMAL) {
                try {
                    deputyId = player.getId();
                    players.put(deputyId, SnakesProto.GamePlayer.newBuilder(players.get(deputyId)).setRole(SnakesProto.NodeRole.DEPUTY).build());
                    sendRoleChange(SnakesProto.NodeRole.DEPUTY, SnakesProto.NodeRole.MASTER, player.getId()); // Мастер сообщает узлу, что тот теперь депути
                    break;
                } catch (IOException e) {
                    System.err.println("Error sending RoleChangeMsg: " + e.getMessage());
                }
            }
        }
    }
    private void handlePlayerDisconnection(int disconnectedPlayerId) {
        System.out.println("[FAILOVER] Handling disconnection for playerId=" + disconnectedPlayerId
                + " myRole=" + nodeRole + " masterId=" + masterId + " deputyId=" + deputyId);

        if (disconnectedPlayerId == -1) {
            System.err.println("Invalid player ID for disconnection: " + disconnectedPlayerId);
            return;
        }

        // ===== NORMAL =====
        if (nodeRole == SnakesProto.NodeRole.NORMAL) {

            // если отвалился именно мастер — переключаемся на deputy
            if (disconnectedPlayerId == masterId) {
                if (deputyId != -1 && players.get(deputyId) != null) {
                    masterId = deputyId;
                    serverPort = players.get(deputyId).getPort();
                    try {
                        serverAddress = InetAddress.getByName(players.get(deputyId).getIpAddress());
                    } catch (UnknownHostException e) {
                        System.err.println("Cannot resolve deputy address: " + e.getMessage());
                        return;
                    }

                    onMasterChanged(new InetSocketAddress(serverAddress, serverPort), masterId);

                    System.out.println("[FAILOVER] NORMAL switched to DEPUTY as MASTER: id=" + masterId
                            + " addr=" + serverAddress + ":" + serverPort);
                } else {
                    System.err.println("Master disconnected, and there is no deputy (or deputy unknown). Game over.");
                }
            }

            // по ТЗ: его змея становится ZOMBIE (а не исчезает)
            disconnectPlayerToZombie(disconnectedPlayerId);
            return;
        }

        // ===== MASTER =====
        if (nodeRole == SnakesProto.NodeRole.MASTER) {
            if (disconnectedPlayerId == deputyId) {
                deputyId = -1;
                // по ТЗ: deputy, который выпал, становится viewer+zombie
                disconnectPlayerToZombie(disconnectedPlayerId);
                selectNewDeputy();
                System.out.println("MASTER processed DEPUTY disconnection: " + disconnectedPlayerId);
                return;
            }

            // обычный игрок выпал
            disconnectPlayerToZombie(disconnectedPlayerId);
            System.out.println("MASTER processed NORMAL disconnection: " + disconnectedPlayerId);
            return;
        }

        // ===== DEPUTY =====
        if (nodeRole == SnakesProto.NodeRole.DEPUTY) {

            // если выпал мастер — я становлюсь мастером
            if (disconnectedPlayerId == masterId) {
                pingSender.interrupt();

                masterId = playerId;
                nodeRole = SnakesProto.NodeRole.MASTER;

                // мой адрес/порт (у тебя они лежат в players по моему playerId)
                serverPort = players.get(playerId).getPort();
                try {
                    serverAddress = InetAddress.getByName(players.get(playerId).getIpAddress());
                } catch (UnknownHostException e) {
                    throw new RuntimeException(e);
                }

                players.put(masterId, SnakesProto.GamePlayer.newBuilder(players.get(masterId))
                        .setRole(SnakesProto.NodeRole.MASTER)
                        .build());

                // ВАЖНО: обновляем текущего мастера (иначе state будут игнориться)
                onMasterChanged(new InetSocketAddress(serverAddress, serverPort), masterId);

                // старый мастер стал viewer+zombie
                disconnectPlayerToZombie(disconnectedPlayerId);

                deputyId = -1;
                selectNewDeputy();

                System.out.println("[FAILOVER] I am new MASTER id=" + masterId
                        + " addr=" + serverAddress + ":" + serverPort + " deputyId=" + deputyId);

                synchronized (lockGameField) {
                    GameLogic.editGameFieldFromState(gameField, lastStateMsg);
                    synchronized (lockSnakeGame) {
                        snakeGame = new GameLogic(gameField, lockGameField);
                    }
                }

                currentMaxId = players.values().stream()
                        .map(SnakesProto.GamePlayer::getId)
                        .max(Integer::compare)
                        .orElse(playerId);

                startServerThreads();
                return;
            }

            // выпал кто-то другой (не мастер)
            disconnectPlayerToZombie(disconnectedPlayerId);
            return;
        }

        System.err.println("STRANGE disconnectedPlayerId: I am VIEWER, disconnectedPlayerId is "
                + disconnectedPlayerId + "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    private void sendGameMessage(SnakesProto.GameMessage gameMessage, InetAddress address, int port) {
        SnakesProto.GameMessage message = SnakesProto.GameMessage.newBuilder(gameMessage)
                .setSenderId(this.playerId)
                .setReceiverId(getPlayerIdByAddress(address, port))
                .build();
        try {
            byte[] buffer = message.toByteArray();
            DatagramPacket data = new DatagramPacket(buffer, buffer.length, address, port);
            if (players.get(message.getReceiverId()) != null) {
                System.out.println("Send message " + message.getTypeCase() + "(rec: " + message.getReceiverId() + ", sen: " + message.getSenderId() + ", seq: " + message.getMsgSeq() + ") to " + address + ":" + port + "(" + players.get(message.getReceiverId()).getRole() + ")");
            } else {
                System.out.println("Send message " + message.getTypeCase() + "(rec: " + message.getReceiverId() + ", sen: " + message.getSenderId() + ", seq: " + message.getMsgSeq() + ") to " + address + ":" + port);
            }
            if (message.getTypeCase() == SnakesProto.GameMessage.TypeCase.ANNOUNCEMENT) {
                socket.send(data);
            } else {
                socket.send(data);
                if (message.getReceiverId() == -1 || message.getTypeCase() == SnakesProto.GameMessage.TypeCase.ACK) return;
                if (sentMessages.get(message.getReceiverId()) == null) {
                    sentMessages.put(message.getReceiverId(), new ConcurrentHashMap<>());
                }
                if (sentMessages.get(message.getReceiverId()).get(message.getMsgSeq()) == null)
                    sentMessages.get(message.getReceiverId()).put(message.getMsgSeq(), new MessageInfo(message, address, port, System.currentTimeMillis()));
            }
        } catch (IOException e) {
            System.err.println("Error sending message: " + e.getMessage());
            int player = getPlayerIdByAddress(address, port);
            handlePlayerDisconnection(player);
        } catch (Exception e) {
            int player = message.getReceiverId();
            players.remove(player);
        }
    }
    private int getPlayerIdByAddress(InetAddress address, int port) {
        return addressToPlayerId.getOrDefault(new InetSocketAddress(address, port), -1);
    }
    private int addNewPlayer(String playerName, InetAddress address, int port, SnakesProto.NodeRole requestedRole) {
        int playerId = ++currentMaxId;
        System.out.println("ROLE " + requestedRole);
        if (requestedRole == SnakesProto.NodeRole.MASTER && this.nodeRole != SnakesProto.NodeRole.MASTER) {
            return -1;
        }
        if (requestedRole == SnakesProto.NodeRole.DEPUTY) {
            if (deputyId == -1) {
                deputyId = playerId;
            } else {
                return -1;
            }
        }
        SnakesProto.GamePlayer player = SnakesProto.GamePlayer.newBuilder()
                .setId(playerId)
                .setName(playerName)
                .setRole(requestedRole)
                .setIpAddress(address.getHostAddress())
                .setPort(port)
                .setScore(0)
                .build();
        players.put(playerId, player);
        addressToPlayerId.put(new InetSocketAddress(address, port), playerId);
        sentMessages.put(playerId, new ConcurrentHashMap<>());
        return playerId;
    }
    public synchronized void stop() {
        if (nodeRole == SnakesProto.NodeRole.MASTER) {
            try {
                if (deputyId != -1) {
                    sendRoleChange(SnakesProto.NodeRole.MASTER, SnakesProto.NodeRole.MASTER, deputyId);
                }
            } catch(IOException e) {
                System.err.println("Error to send role change!");
                e.printStackTrace();
            }
        }

        if (socket != null) {
            socket.close();
        }

        if (messageReceiverLoop != null) messageReceiverLoop.interrupt();
        if (announcementSendThread != null) announcementSendThread.interrupt();
        if (gameLoop != null) gameLoop.interrupt();
        if (messageResenderThread != null) messageResenderThread.interrupt();
        if (pingSender != null) pingSender.interrupt();
    }
    private void onMasterChanged(InetSocketAddress newMasterAddr, int newMasterId) {
        System.out.println("[FAILOVER] MASTER -> id=" + newMasterId + " addr=" + newMasterAddr);

        currentMasterAddr = newMasterAddr;
        currentMasterId = newMasterId;

        // новый мастер может начать state_order "с другого места"
        lastStateOrder = -1;

        // дедуп к старой эпохе может мешать
        lastMsgSeqReceived.clear();

        // если хочешь минимум — можно пока не переносить pending (позже сделаем правильно)
        // resendAllPendingToNewMaster(newMasterAddr);
    }

}
