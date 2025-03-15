# search.py
# ---------
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


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem: SearchProblem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:
    """
    "*** YOUR CODE HERE ***"

    """
    Algoritmo:
        Enquanto não for o resultado
            Se vertice atual ainda nao visitado
                Marca vertice como visitado
                Para cada sucessor do vertice      #Pega proximos caminhos
                    empilha sucessores
                    empilha caminho dos sucessores
                
            vai pra um sucessor x (Tira do topo da pilha)
            salva caminho do sucessor x (tira do topo da pilha)
    """

    atual = problem.getStartState()     #Coloca atual no inicio
    proximos = util.Stack()             #Vertices para avancar
    proximos_path = util.Stack()        #Caminhos dos vertices para avancar
    ja_passado = []                     #Vertices que ja passou
    caminho_final = []                  #Caminho a retornar

    while not problem.isGoalState(atual):
        if atual not in ja_passado:
            ja_passado.append(atual)
            for prox_no in problem.getSuccessors(atual):
                proximos.push(prox_no[0])   #Empilha proximos caminhos possiveis
                proximos_path.push(caminho_final + [prox_no[1]])

        caminho_final = proximos_path.pop()
        atual = proximos.pop()

    return caminho_final

def breadthFirstSearch(problem: SearchProblem):
    """Search the shallowest nodes in the search tree first."""
    "*** YOUR CODE HERE ***"

    """
    Algoritmo:
        Enquanto não for o resultado
            Se vertice atual ainda nao visitado
                Marca vertice como visitado
                Para cada sucessor do vertice      #Pega proximos caminhos
                    empilha sucessores
                    empilha caminho dos sucessores
                
            vai pra um sucessor x (remove da lista)
            salva caminho do sucessor x (remove da lista)
    """

    atual = problem.getStartState()     #Coloca atual no inicio
    proximos = util.Queue()             #Vertices para avancar
    proximos_path = util.Queue()        #Caminhos dos vertices para avancar
    ja_passado = []                     #Vertices que ja passou
    caminho_final = []                  #Caminho a retornar

    while not problem.isGoalState(atual):
        if atual not in ja_passado:
            ja_passado.append(atual)
            for prox_no in problem.getSuccessors(atual):
                proximos.push(prox_no[0])   #Empilha proximos caminhos possiveis
                proximos_path.push(caminho_final + [prox_no[1]])

        caminho_final = proximos_path.pop()
        atual = proximos.pop()

    return caminho_final

def uniformCostSearch(problem: SearchProblem):
    """Search the node of least total cost first."""
    "*** YOUR CODE HERE ***"

    """
    Algoritmo:
        Enquanto não for o resultado
            Se vertice atual ainda nao visitado
                Marca vertice como visitado
                Para cada sucessor do vertice      #Pega proximos Caminhos
                    calcula custo
                    empilha (sucessores, custo)
                    empilha (caminho dos sucessores, custo)
                
            vai pra um sucessor x (desempilha sucessor)
            salva caminho do sucessor x (desempilha caminho)
    """

    atual = problem.getStartState()         #Coloca atual no inicio
    proximos = util.PriorityQueue()         #Vertices para avancar
    proximos_path = util.PriorityQueue()    #Caminhos dos vertices para avancar
    ja_passado = []                         #Vertices que ja passou
    caminho_final = []                      #Caminho a retornar

    while not problem.isGoalState(atual):
        if atual not in ja_passado:
            ja_passado.append(atual)
            for prox_no in problem.getSuccessors(atual):        #[(coordenates), direction, cost]
                custo_acao = problem.getCostOfActions(caminho_final + [prox_no[1]])
                proximos.push(prox_no[0], custo_acao)   #Empilha proximos caminhos possiveis
                proximos_path.push(caminho_final + [prox_no[1]], custo_acao)

        caminho_final = proximos_path.pop()
        atual = proximos.pop()

    return caminho_final

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem: SearchProblem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"

    """
    Algoritmo:
        Enquanto não for o resultado{
            Se vertice atual ainda nao visitado{
                Marca vertice como visitado
                Para cada sucessor do vertice{      #Pega proximos caminhos
                    calcula custo da acao 
                    soma com o custo da heuristica
                    empilha (sucessor, custo total)
                    empilha (caminho dos sucessores, custo total)
                }
            }
            vai pra um sucessor x (desempilha sucessor)
            salva caminho do sucessor x (desempilha caminho)
        }
    """

    atual = problem.getStartState()     #Coloca atual no inicio
    proximos = util.PriorityQueue()     #Vertices para avancar
    proximos_path = util.PriorityQueue()        #Caminhos dos vertices para avancar
    ja_passado = []                     #Vertices que ja passou
    caminho_final = []                  #Caminho a retornar

    while not problem.isGoalState(atual):
        if atual not in ja_passado:
            ja_passado.append(atual)
            for prox_no in problem.getSuccessors(atual):
                custo_acao = problem.getCostOfActions(caminho_final + [prox_no[1]])
                custo_acao += heuristic(prox_no[0], problem)
                proximos.push(prox_no[0], custo_acao)   #Empilha proximos caminhos possiveis
                proximos_path.push(caminho_final + [prox_no[1]], custo_acao)

        caminho_final = proximos_path.pop()
        atual = proximos.pop()
    return caminho_final

# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
