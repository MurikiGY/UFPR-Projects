# qlearningAgents.py
# ------------------
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


from game import *
from learningAgents import ReinforcementAgent
from featureExtractors import *

import gridworld

import random,util,math
import copy

class QLearningAgent(ReinforcementAgent):
    """
      Q-Learning Agent
      Functions you should fill in:
        - computeValueFromQValues
        - computeActionFromQValues
        - getQValue
        - getAction
        - update
      Instance variables you have access to
        - self.epsilon (exploration prob)
        - self.alpha (learning rate)
        - self.discount (discount rate)
      Functions you should use
        - self.getLegalActions(state)
          which returns legal actions for a state
    """
    def __init__(self, **args):
        "You can initialize Q-values here..."
        ReinforcementAgent.__init__(self, **args)

        "*** YOUR CODE HERE ***"
        " Inicializa dicionario de estado-valor"
        self.qvalue = util.Counter()

    def getQValue(self, state, action):
        """
          Returns Q(state,action)
          Should return 0.0 if we have never seen a state
          or the Q node value otherwise
        """
        "*** YOUR CODE HERE ***"
        return self.qvalue[state, action]

    def computeValueFromQValues(self, state):
        """
          Returns max_action Q(state,action)
          where the max is over legal actions.  Note that if
          there are no legal actions, which is the case at the
          terminal state, you should return a value of 0.0.
        """
        "*** YOUR CODE HERE ***"
        maior_valor = -999999
        maior_acao = None
        """
            Para cada uma das possiveis acoes
            Retorne a que getQValue retorna o maior valor
        """
        acoes_possiveis = self.getLegalActions(state)
        for acao in acoes_possiveis:
            if self.getQValue(state, acao) > maior_valor:
                maior_valor = self.getQValue(state, acao)
                maior_acao = acao

        " Se nao encontrou nenhuma acao "
        if maior_acao == None:
            return 0.0
        return maior_valor

    def computeActionFromQValues(self, state):
        """
          Compute the best action to take in a state.  Note that if there
          are no legal actions, which is the case at the terminal state,
          you should return None.
        """
        "*** YOUR CODE HERE ***"

        acoes_possiveis = self.getLegalActions(state)
        acao_otima = None
        if len(acoes_possiveis) > 0:
            maior_q = -99999
            for acao in acoes_possiveis:
                valor_q = self.getQValue(state, acao)
                if valor_q > maior_q:
                    maior_q = valor_q
                    acao_otima = acao
            return acao_otima
        else:
            return None

        #if len(acoes_possiveis) == 0:
        #    return None
        #else:
        #    highest_q_value = -999999
        #    for act in acoes_possiveis:
        #        if self.getQValue(state, act) > highest_q_value:
        #            highest_q_value = self.getQValue(state, act)
        #            optimal_action = act
        #    return optimal_action

    def getAction(self, state):
        """
          Compute the action to take in the current state.  With
          probability self.epsilon, we should take a random action and
          take the best policy action otherwise.  Note that if there are
          no legal actions, which is the case at the terminal state, you
          should choose None as the action.
          HINT: You might want to use util.flipCoin(prob)
          HINT: To pick randomly from a list, use random.choice(list)
        """
        # Pick Action
        legalActions = self.getLegalActions(state)
        action = None
        "*** YOUR CODE HERE ***"

        """
            Pega uma acao aleatoria
            retorna
        """
        aleatorio = util.flipCoin(self.epsilon)

        #if aleatorio:
        #    action = random.choice(legalActions)
        #else:
        #    action = self.computeActionFromQValues(state)

        action = random.choice(legalActions) if aleatorio else self.computeActionFromQValues(state)

        return action

    def update(self, state, action, nextState, reward: float):
        """
          The parent class calls this to observe a
          state = action => nextState and reward transition.
          You should do your Q-Value update here
          NOTE: You should never call this function,
          it will be called on your behalf
        """
        "*** YOUR CODE HERE ***"
        desconto = self.discount
        valor_comp = self.computeValueFromQValues(nextState)

        """
            Calcula sample
        """
        exemplo = reward + (desconto*valor_comp)
        tupla_idx = (state, action)
        valor_q = self.getQValue(state, action)
        self.qvalue[tupla_idx] = (1 - self.alpha)*valor_q + self.alpha*exemplo

        #sample = reward + self.discount * self.computeValueFromQValues(nextState)
        #key = (state, action)
        #q_value = self.getQValue(state, action)
        #self.qvalue[key] = (1 - self.alpha) * q_value + self.alpha * sample

    def getPolicy(self, state):
        return self.computeActionFromQValues(state)

    def getValue(self, state):
        return self.computeValueFromQValues(state)


class PacmanQAgent(QLearningAgent):
    "Exactly the same as QLearningAgent, but with different default parameters"

    def __init__(self, epsilon=0.05,gamma=0.8,alpha=0.2, numTraining=0, **args):
        """
        These default parameters can be changed from the pacman.py command line.
        For example, to change the exploration rate, try:
            python pacman.py -p PacmanQLearningAgent -a epsilon=0.1
        alpha    - learning rate
        epsilon  - exploration rate
        gamma    - discount factor
        numTraining - number of training episodes, i.e. no learning after these many episodes
        """
        args['epsilon'] = epsilon
        args['gamma'] = gamma
        args['alpha'] = alpha
        args['numTraining'] = numTraining
        self.index = 0  # This is always Pacman
        QLearningAgent.__init__(self, **args)

    def getAction(self, state):
        """
        Simply calls the getAction method of QLearningAgent and then
        informs parent of action for Pacman.  Do not change or remove this
        method.
        """
        action = QLearningAgent.getAction(self,state)
        self.doAction(state,action)
        return action

class ApproximateQAgent(PacmanQAgent):
    """
       ApproximateQLearningAgent
       You should only have to overwrite getQValue
       and update.  All other QLearningAgent functions
       should work as is.
    """
    def __init__(self, extractor='IdentityExtractor', **args):
        self.featExtractor = util.lookup(extractor, globals())()
        PacmanQAgent.__init__(self, **args)
        self.weights = util.Counter()

    def getWeights(self):
        return self.weights

    def getQValue(self, state, action):
        """
          Should return Q(state,action) = w * featureVector
          where * is the dotProduct operator
        """
        "*** YOUR CODE HERE ***"
        w = self.getWeights()
        vetorPropriedades = self.featExtractor.getFeatures(state,action)

        return w * vetorPropriedades
        #util.raiseNotDefined()

    def update(self, state, action, nextState, reward: float):
        """
           Should update your weights based on transition
        """
        "*** YOUR CODE HERE ***"
        propriedades_estado = self.featExtractor.getFeatures(state, action)
        diferenca = reward + self.discount * self.getValue(nextState) - self.getQValue(state, action)

        for feature in propriedades_estado:
            self.weights[feature] += self.alpha * diferenca * propriedades_estado[feature]

        #util.raiseNotDefined()

    def final(self, state):
        """Called at the end of each game."""
        # call the super-class final method
        PacmanQAgent.final(self, state)

        # did we finish training?
        if self.episodesSoFar == self.numTraining:
            # you might want to print your weights here for debugging
            "*** YOUR CODE HERE ***"
            pass
