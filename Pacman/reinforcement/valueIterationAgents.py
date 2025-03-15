# valueIterationAgents.py
# -----------------------
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


# valueIterationAgents.py
# -----------------------
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


import mdp, util

from learningAgents import ValueEstimationAgent
import collections

class ValueIterationAgent(ValueEstimationAgent):
    """
        * Please read learningAgents.py before reading this.*

        A ValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs value iteration
        for a given number of iterations using the supplied
        discount factor.
    """
    def __init__(self, mdp: mdp.MarkovDecisionProcess, discount = 0.9, iterations = 100):
        """
          Your value iteration agent should take an mdp on
          construction, run the indicated number of iterations
          and then act according to the resulting policy.

          Some useful mdp methods you will use:
              mdp.getStates()
              mdp.getPossibleActions(state)
              mdp.getTransitionStatesAndProbs(state, action)
              mdp.getReward(state, action, nextState)
              mdp.isTerminal(state)
        """
        self.mdp = mdp
        self.discount = discount
        self.iterations = iterations
        self.values = util.Counter() # A Counter is a dict with default 0
        self.runValueIteration()

    def runValueIteration(self):
        """
          Run the value iteration algorithm. Note that in standard
          value iteration, V_k+1(...) depends on V_k(...)'s.
        """
        "*** YOUR CODE HERE ***"

        """
        Objetivo: Popular self.values com as probabilidades de cada estado
        """

        """ Para cada uma das iteracoes """
        #for itr in range(self.iterations):
        #    """ Armazena os valores dos estados a cada iteracao """
        #    valor_estado = util.Counter()
        #    for estado in self.mdp.getStates():
        #        if not self.mdp.isTerminal(estado):
        #            maior_valor = 0
        #            for acao in self.mdp.getPossibleActions(estado):
        #                valor = 0

        #                """ Soma a probabilidade de todos os possiveis estados """
        #                for esp in self.mdp.getTransitionStatesAndProbs(estado, acao):
        #                    recompensa = self.mdp.getReward(estado, acao, esp[1])
        #                    posicao = esp[0]
        #                    valor_anterior = self.values[posicao]
        #                    desconto = self.discount
        #                    valor += esp[1] * (recompensa + desconto * valor_anterior)

        #                maior_valor = max(maior_valor, valor)

        #            valor_estado = maior_valor

        #    self.values = valor_estado

        for i in range(self.iterations):
            dict_posicao_valor = util.Counter()
            estados = self.mdp.getStates()
            """ Iterando sobre cada uma das posicoes do grid """
            for posicao in estados or []:
                """ 
                    Se a posicao nao for terminal, então calcula probabilidade
                    Do contrario, a posicao do dicionario recebe zero por defaulr de util.Counter()
                """
                if not self.mdp.isTerminal(posicao):
                    maior_valor = -99999

                    for act in self.mdp.getPossibleActions(posicao) or []:
                        valor = 0
                        for estado_prob in self.mdp.getTransitionStatesAndProbs(posicao, act) or []:
                            recompensa = self.mdp.getReward(posicao, act, estado_prob[1])
                            desconto = self.discount
                            probabilidade = self.values[estado_prob[0]]
                            valor += estado_prob[1] * (recompensa + desconto * probabilidade)
                        maior_valor = max(maior_valor, valor)

                    if maior_valor != -99999:
                        dict_posicao_valor[posicao] = maior_valor

            self.values = dict_posicao_valor

    def getValue(self, state):
        """
          Return the value of the state (computed in __init__).
        """
        return self.values[state]

    def computeQValueFromValues(self, state, action):
        """
          Compute the Q-value of action in state from the
          value function stored in self.values.
        """
        "*** YOUR CODE HERE ***"
        sum = 0
        for nxt, p in self.mdp.getTransitionStatesAndProbs(state, action) or []:
            sum += p * (self.mdp.getReward(state, action, nxt) + self.discount * self.values[nxt])
        return sum

        #util.raiseNotDefined()

    def computeActionFromValues(self, state):
        """
          The policy is the best action in the given state
          according to the values currently stored in self.values.

          You may break ties any way you see fit.  Note that if
          there are no legal actions, which is the case at the
          terminal state, you should return None.
        """
        "*** YOUR CODE HERE ***"
        if self.mdp.isTerminal(state):
            return None

        acoes = self.mdp.getPossibleActions(state)
        valor = -99999
        retorna_acao = ''
        for direcao in acoes or []:
            calculo = self.computeQValueFromValues(state, direcao)
            if calculo > valor:
                valor = calculo
                retorna_acao = direcao

        return retorna_acao
        #return max(self.mdp.getPossibleActions(state) or [], key=lambda x: self.computeQValueFromValues(state, x))
        #util.raiseNotDefined()

    def getPolicy(self, state):
        return self.computeActionFromValues(state)

    def getAction(self, state):
        "Returns the policy at the state (no exploration)."
        return self.computeActionFromValues(state)

    def getQValue(self, state, action):
        return self.computeQValueFromValues(state, action)
