if [ $# != 3 ]; then
    echo "Usage: ./test_params.sh <lambda> <stepsize> <N>"
    return
fi
export LIBRLAGENT_LAMBDA=$1
export LIBRLAGENT_STEPSIZE=$2
# ./run.py -N $3 -E environments/tictactoe/TictactoeEnvironment -X ChristosExperiment -A ../examplecode/environmentschristos/SarsaAgent
# ./run.py -N $3 -E environments/contextbandit/ContextBanditEnvironment -X ChristosExperiment
./run.py -N $3 -E environments/tictactoe/TictactoeEnvironment -X ChristosExperiment
# ./run.py -N $3 -E environments/tictactoe/TictactoeEnvironment -X MyExperiment
unset LIBRLAGENT_LAMBDA
unset LIBRLAGENT_STEPSIZE
