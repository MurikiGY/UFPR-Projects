# multiAgents.py
# --------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


from re import A
from util import manhattanDistance, sample
from game import Directions
import random, util

from game import Agent
from pacman import GameState

class ReflexAgent(Agent):
    """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
    """


    def getAction(self, gameState: GameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState: GameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood()
        newGhostStates = successorGameState.getGhostStates()
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

        "*** YOUR CODE HERE ***"
        """
        Calcula a menor distancia aos proximos estados do fantasma (sucessorState)
        """
        listaFantasma = []
        for estado in newGhostStates:
            listaFantasma.append(manhattanDistance(estado.getPosition(), newPos))
        fantasMenorDist = min(listaFantasma)

        """
        Busca a comida mais proxima a posicao do pacman (currentState)
        """
        listaComida = []
        for foodPoint in currentGameState.getFood().asList():
            listaComida.append(manhattanDistance(foodPoint, currentGameState.getPacmanPosition()))
        comidaMaisProx = min(listaComida)

        foodNew = []
        for foodPoint in newFood.asList():
            foodNew.append(manhattanDistance(foodPoint, newPos))
        if (not foodNew):
            newNearestFood = 0
        else:
            newNearestFood = min(foodNew)

        """
        Se esta do lado do fantasma
        """
        if action == Directions.STOP or fantasMenorDist <= 1: 
            return 0

        """
        se ficou parado
        """
        if action == currentGameState.getPacmanState().getDirection(): 
            return 2
        """
        Se a distancia a comida mais proxima diminuiu
        """
        if comidaMaisProx - newNearestFood > 0:
            return 4

        """
        Se o score aumenta
        """
        if successorGameState.getScore() - currentGameState.getScore() > 0: 
            return 8
        return 1

def scoreEvaluationFunction(currentGameState: GameState):
    """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
    Your minimax agent (question 2)
    """

    def getAction(self, gameState: GameState):
        """
        Returns the minimax action from the current gameState using self.depth
        and self.evaluationFunction.

        Here are some method calls that might be useful when implementing minimax.

        gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

        gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

        gameState.getNumAgents():
        Returns the total number of agents in the game

        gameState.isWin():
        Returns whether or not the game state is a winning state

        gameState.isLose():
        Returns whether or not the game state is a losing state
        """
        "*** YOUR CODE HERE ***"

        """
        Busca a menor evaluationFunction dos nos filhos
        os nos filhos podem ser de maximizar ou minimizar
        """
        def minimiza(state, agente, depth):
            """
            Verifica quais acoes o agente pode fazer
            """
            actions = state.getLegalActions(agente)
            if not actions: return self.evaluationFunction(state)

            successors = []
            """
            Se o agente atual eh o ultimo fantasma a se mover
            entao o proximo eh o pacman, logo maximiza
            """
            if agente == state.getNumAgents() - 1:
                for action in actions:
                    successors.append(maximiza(state.generateSuccessor(agente, action), depth))
                return min(successors)
            else:
                for action in actions:
                    successors.append(minimiza(state.generateSuccessor(agente, action), agente+1, depth))
                return min(successors)

        def maximiza(state, depth):
            """
            Verifica quais acoes o pacman pode fazer
            """
            actions = state.getLegalActions(0)

            """
            Se chegou no fim da arvore
            ou nao ha mais acoes
            """
            if depth == self.depth or not actions:
                return self.evaluationFunction(state)

            successors = []
            for action in actions:
                successors.append(minimiza(state.generateSuccessor(0, action), 1, depth + 1))
            return max(successors)

        #pontuacao = util.PriorityQueue()
        #actions = gameState.getLegalActions(0)
        #for move in actions:
        #    proximoEstado = gameState.generateSuccessor(0, move)
        #    pontuacao.push(move, minimiza(proximoEstado, 1, 1))
        #return pontuacao.pop()
        return max(gameState.getLegalActions(0), key=lambda action: minimiza(gameState.generateSuccessor(0, action), 1, 1))

class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState: GameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"

        """
        Funcao chamada pelo ultimo fantasma a se mover
        Maximizacao realizada pelo pacman
        """
        def maximiza(gameState,currDepth,alpha, beta):
            if currDepth==self.depth or gameState.isWin() or gameState.isLose():
                return self.evaluationFunction(gameState)

            maxvalue = -999999
            newAlpha = alpha
            actions = gameState.getLegalActions(0)
            """
            beta eh o melhor encontrado ate entao
            Para cada um dos nos filhos
            verifica se e melhor que o beta
            """
            for action in actions:
                successor= gameState.generateSuccessor(0,action)
                maxvalue = max(maxvalue,minimiza(successor,currDepth,1,newAlpha,beta))
                if maxvalue > beta:
                    return maxvalue
                newAlpha = max(newAlpha, maxvalue)
            return maxvalue

        """
        Testa se pacman ou fantasma
        se pacman maximiza nos filhos
        se fantasma, testa se algum filho eh menor do que o melhor ja encontrado
        """
        def minimiza(gameState,depth,agent,alpha,beta):
            if gameState.isWin() or gameState.isLose():
                return self.evaluationFunction(gameState)

            minFound = 999999
            newBeta = beta
            actions = gameState.getLegalActions(agent)
            for action in actions:
                successor = gameState.generateSuccessor(agent,action)
                """
                Se o proximo a se mover for o Pacman
                """
                if agent == (gameState.getNumAgents()-1):
                    minFound = min(minFound, maximiza(successor,depth+1,alpha,newBeta))
                    if minFound < alpha:
                        return minFound
                    newBeta = min(minFound, newBeta)
                else:
                    """
                    fantasma vai tentar minimizar
                    se algum no filho for menor que o melhor encontrado(alfa)
                    nao adianta continuar, corta fora
                    """
                    minFound = min(minFound,minimiza(successor,depth,agent+1,alpha,newBeta))
                    if minFound < alpha:
                        return minFound
                    newBeta = min(minFound, newBeta)
            return minFound

        beta = 999999  #Pior caso para o fantasma
        alpha = -999999  #Pior caso para o pacman
        pontuacao = -999999
        acoes = gameState.getLegalActions(0)
        caminho = ''
        """
        Para cada uma das possiveis acoes
        """
        for move in acoes:
            nextState = gameState.generateSuccessor(0, move)
            newScore = minimiza(nextState, 0, 1, alpha, beta)

            """
            Salva em pontuacao o maior score que encontrar
            """
            if newScore > pontuacao:
                pontuacao = newScore
                caminho = move

            if newScore > beta:
                return caminho
            alpha = max(newScore, alpha)
        return caminho

class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState: GameState):
        """
        Returns the expectimax action using self.depth and self.evaluationFunction

        All ghosts should be modeled as choosing uniformly at random from their
        legal moves.
        """
        "*** YOUR CODE HERE ***"

        def expect(state, agent, depth):
            """
            Pega todos os possiveis proximos passos do agente
            """
            actions = state.getLegalActions(agent)
            if not actions: return self.evaluationFunction(state)

            """
            soma todos os filhos e divide pelo numero de filhos
            """
            soma = 0
            razao = 1.0/len(actions)
            for move in actions:
                newState = state.generateSuccessor(agent, move)
                if agent == state.getNumAgents() - 1:
                    soma += maximiza(newState, depth) * razao 
                else:
                    soma += expect(newState, agent + 1, depth) * razao
            return soma

        def maximiza(state, depth):
            """
            Pega todos os possiveis proximos passos do pacman
            """
            actions = state.getLegalActions(0)
            if depth == self.depth or not actions:
                return self.evaluationFunction(state)

            """
            Tipicamente retorna o maior dos filhos
            """
            successors = []
            for move in actions:
                successors.append(expect(state.generateSuccessor(0, move), 1, depth + 1))
            return max(successors)

        return max(gameState.getLegalActions(), key=lambda action: expect(gameState.generateSuccessor(0, action), 1, 1))

def betterEvaluationFunction(currentGameState: GameState):
    """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <write something here so we know what you did>
    """
    "*** YOUR CODE HERE ***"
    
    """
    Informacoes do pacman
    """
    posicaoPacman = currentGameState.getPacmanPosition()
    pontuacaoAtual = currentGameState.getScore()

    """
    informacoes dos fantasmas
    """
    newGhostStates = currentGameState.getGhostStates()
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]
    ghosts = [] #Posicao de cada fantasma
    ghostsDist = [0] #Distancia a cada fantasmas

    """
    informacoes das comidas
    """
    newFood = currentGameState.getFood()
    listaComidas = newFood.asList()
    distanciaComidas = [0]  #Distancia a cada comida
    
    """
    Calcula posicao dos fantasmas e a distancia
    """
    for ghost in newGhostStates:
        ghosts.append(ghost.getPosition())
    for ghost in ghosts:
        ghostsDist.append(manhattanDistance(posicaoPacman,ghost))

    """
    Calcula a distancia a cada comida
    """
    for food in listaComidas:
        distanciaComidas.append(manhattanDistance(posicaoPacman, food))

    """
    Calcula score maximo atual possivel
    """
    pontuacao =  pontuacaoAtual + len(newFood.asList(False))

    if sum(distanciaComidas) > 0: pontuacao += 1.0 / sum(distanciaComidas)

    if min(ghostsDist) > min(distanciaComidas): pontuacao += max(distanciaComidas)

    #"""
    #aumenta a pontuacao se tiver mais comida proxima do que fantasma
    #"""
    #if sum(distanciaComidas) < sum(ghostsDist): pontuacao += 1.0 / (sum(distanciaComidas)+1)

    #"""
    #Caso tenha poucas comidas, tenta compensar a falta da soma de distancias
    #"""
    #if (len(listaComidas) < 5) and (sum(ghostsDist) > 5): pontuacao += 1.0 / (sum(ghostsDist))

    if sum(newScaredTimes) > 0:    
        pontuacao += sum(newScaredTimes) + (-sum(ghostsDist)) + (-len(currentGameState.getCapsules()))
    else:
        pontuacao += sum(ghostsDist) + len(currentGameState.getCapsules())
    return pontuacao 

# Abbreviation
better = betterEvaluationFunction
