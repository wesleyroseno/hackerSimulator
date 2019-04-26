#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>
#include <fstream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <stdlib.h>
#include <mensagens.h>

/*
 * O uso do boost ajuda na hora do split
 * eh preciso instalar a biblioteca libboost-dev
 */
using namespace std;
using namespace boost;

/*
 * Criando diretorios padroes
 * --------------------------
 * Usa mapas para armazenar os subdiretorios e os arquivos
 * Arquivos terao como chave o nome e valor o conteudo
 * Diretorios terao como chave o nome e valor o subdiretorio
 *
 */
struct Diretorio {
	map<string, string> arquivos;
	map<string, Diretorio> subdiretorios;
	Diretorio *pai;
};

struct TuplaDirArq {
	Diretorio dir;
	string arq;
};

// Mapa de ips que apontam para servidores
map<string, Diretorio> servidores;

// Diretorio atual no qual o programa esta
Diretorio _dirAtual;

// Servidor onde o usuario esta
Diretorio _server;

/*
 * Funcao ls, recebe uma struct (o diretorio atual)
 * imprime os diretorios em ordem alfabetica e
 * abaixo os arquivos em ordem alfabetica.
 *
 */
string ls(Diretorio& local) {
	string saida;
	for (auto& x: local.subdiretorios) {
		saida.append(x.first + "/");
		saida.append("\n");
	}
	for (auto& x: local.arquivos) {
		saida.append(x.first);
		saida.append("\n");
	}

	return saida;
}

/*
 * Funcao cat, recebe um diretorio e um nome
 * de arquivo e retorna o conteudo do arquivo.
 *
 */
 string cat(Diretorio& dir, string arquivo) {
 	string saida;
	if (dir.arquivos.count(arquivo))
		saida = dir.arquivos[arquivo] + "\n";
	else
		saida = "cat: " + arquivo + ": Arquivo ou diretorio nao encontrado\n";
	return saida;
}

/*
 * Funcao rm, dado um diretório e um arquivo, remove esse arquivo.
 *
 */
string rm(Diretorio& dir, string arquivo) {
	string saida;
	if (dir.arquivos.count(arquivo)) {
		dir.arquivos.erase(arquivo);
	} else
		saida = "rm: nao foi possivel remover " + arquivo + ": Arquivo ou diretorio nao encontrado\n";
	return saida;
}

/*
 * Funcao rmdir, dado um diretorio, remove este.
 *
 */
string rmdir(Diretorio& dir, string dirRemover) {
	string saida;
	if (dir.subdiretorios.count(dirRemover))
		dir.subdiretorios.erase(dirRemover);
	else
		saida = "rm: nao foi possivel remover " + dirRemover + ": Arquivo ou diretorio nao encontrado\n";
	return saida;
} // farei mais algumas variacoes se forem necessarias

/*
 * Funcao cp recebe um arquivo a ser copiado e um diretorio detino para este.
 *
 */
string cp(Diretorio& dirOrigem, Diretorio& dirDestino, string arquivo) {
	string saida;
	if (dirOrigem.arquivos.count(arquivo) && !dirDestino.arquivos.count(arquivo))
			dirDestino.arquivos[arquivo] = dirOrigem.arquivos[arquivo];
	else
		saida = "pensar em uma frase para ca";
	return saida;
}

/*
 * muda o diretorio atual para o diretorio passado como paramentro;
 */
string cd(Diretorio& local, string dir) {
	string saida;
	TuplaDirArq retornaDirAndArq(string dirLoc, Diretorio& dirAtual);

	if (dir.compare("..") == 0){
		_dirAtual = *local.pai;
	}

	else {
		try {
			Diretorio dirDestino;
			TuplaDirArq tupla = retornaDirAndArq(dir, local);
			dirDestino = tupla.dir;
			_dirAtual = dirDestino;

		} catch (char e) {
			saida = "bash: cd: " + dir + ": Arquivo ou diretório não encontrado";
		}
	}

	return saida;
}

/*
 * Comando que limpa a tela.
 */
void clear() {
	system("clear");
}

/*
 * Recebe o comando, olha se ele existe no universo do jo, caso exista
 * faz o processamento necessario na string para gerar o retorno
 * desejado.
 *
 * Retorno - Uma string com a saida desejada ou uma mensagem de erro.
 */
string escolheComando(string comando, Diretorio& diretorio) {

	string comandoLoc;
	string dirLoc;
	if ((comando.compare("ls") != 0) && (comando.compare("clear") != 0)) {
		vector<string> splited;
		split(splited, comando, is_any_of(" "));
 		comandoLoc = splited[0];
 		dirLoc = splited[1];
	}

	else {
		comandoLoc = comando;
	}

	string saida;
	Diretorio dirTemp;
	string arqTemp;
	saida = "Comando '" + comandoLoc + "' desconhecido\n";
 	TuplaDirArq retornaDirAndArq(string dirLoc, Diretorio& dirAtual);

	if (comandoLoc.compare("ls") == 0) {
 		if (comando.length() == 2) {
			saida = ls(diretorio);
		}
		else {
			saida = "O comando ls nao possui parametros\n";
		}
	}

	else if (comandoLoc.compare("cat") == 0) {
 		Diretorio tempLoc = _dirAtual;
		TuplaDirArq temp = retornaDirAndArq(dirLoc, _dirAtual);
		dirTemp = temp.dir;
 		arqTemp = temp.arq;
 		saida = cat(dirTemp, arqTemp);
		_dirAtual = tempLoc;
	}

	else if (comandoLoc.compare("cd") == 0) {
		saida = cd(diretorio, dirLoc);
	}

	else if (comandoLoc.compare("clear") == 0) {
 		if (comando.length() == 5) {
			clear();
			saida = "";
		}
		else {
			saida = "O comando clear nao possui parametros\n";
		}
	}

	else if (comandoLoc.compare("rm") == 0) {
 		saida = rm(diretorio, dirLoc);
	}

	return saida;
 }

/*
 * Recebe uma string onde estara o caminho de onde serah executado um comando
 * esse caminho pode estar no formato xx/yy/oo.txt por exemplo
 * onde o diretorio desejado serah yy e o arquivo oo.txt
 *
 * Retorno - Uma struct semelhante a uma tupla, com o dir e o arquivo desejados, ou
 * apenas o dir desejado, vai depender do comando usado.
 *
 *
 */
TuplaDirArq retornaDirAndArq(string dirLoc, Diretorio& dirAtual) {
	Diretorio *dirReturn;
	TuplaDirArq retorno;
	string arqReturn;
	string comp1("/");
	string comp2("");
	vector<string> spliter(string x);

	if (dirLoc.compare(comp1) != 0) {
		vector<string> dirLocSplited = spliter(dirLoc);
		arqReturn = dirLocSplited[dirLocSplited.size() - 1];

		if (dirLocSplited[0].compare(comp2) == 0) {

			dirReturn = &_server;
			for (int i=1 ; i<dirLocSplited.size() ; i++) {
				if (dirReturn->subdiretorios.count(dirLocSplited[i])) {
					dirReturn = &(dirReturn->subdiretorios[dirLocSplited[i]]);
				}

				else if (!dirReturn->arquivos.count(dirLocSplited[i])) {
					throw invalid_argument( "Diretorio ou Arquivo nao encontrado" );
				}
			}

		}

		else if (dirLocSplited.size() > 1) {

			for (int i=0 ; i<dirLocSplited.size() ; i++) {
				if (dirAtual.subdiretorios.count(dirLocSplited[i])) {
					dirAtual = dirAtual.subdiretorios[dirLocSplited[i]];
				}

				else if (!dirAtual.arquivos.count(dirLocSplited[i])) {
					throw invalid_argument( "Diretorio ou Arquivo nao encontrado" );
				}
			}

			dirReturn = &dirAtual;
		}

		else if (dirAtual.subdiretorios.count(dirLocSplited[0])){
			dirReturn = &(dirAtual.subdiretorios[dirLocSplited[0]]);
		}

		else if (!dirAtual.arquivos.count(dirLocSplited[0])) {
			throw invalid_argument( "Diretorio ou Arquivo nao encontrado" );
		}
	}

	else {
		dirReturn = &_server;
	}

	retorno.arq = arqReturn;
	retorno.dir = *dirReturn;

	return retorno;
}

/*
 * Funcao que separa os nomes de diretorios e arquivos
 * para ajudar na busca, retorna um vetor com os nomes
 * em ordem
 *
 */
vector<string> spliter(string comando) {
	vector<string> saida;

	split(saida, comando, is_any_of("/"));

	return saida;
}

/*
 * Loop principal do sistema
 */
void loopSistema() {
	string comando;
	while (1) {
		cout << "Linux => ";
		getline(cin, comando);

		if (comando.compare("exit") == 0) {
			break;
		}
		else {
			cout << escolheComando(comando, _dirAtual);
		}
	}

}

int main(void) {

	/*
 	* Criando servidores e seus Diretorios/Arquivos
	 */

	// O valoe paiDir armazena o pai do diretorio, para o comando (cd ..) funcionar.

	// Pc do usuario
	Diretorio pcUsuario, home, stash, misc, bin, sys, lib, log;

	// Dentro de home

	misc.pai = &home;
	stash.pai = &home;
	home.subdiretorios["stash"] = stash;
	home.subdiretorios["misc"] = misc;

	// Dentro de log
	log.arquivos["logs.txt"] = "@4455_connection:_from_143.203.86.254";

	// Dentro de bin
	bin.arquivos["x-server.sys"] = "100010001011100011000\n01110001100";
	bin.arquivos["os-config.sys"] = "100010001011111111111\n0111100010001100";
	bin.arquivos["bootcfg.dll"] = "000100010001011100011000\n0111000110000\n1111110";
	bin.arquivos["netcfgx.dll"] = "100111000010001011100011000\n01110111001100";

	// Definindo os pais dos diretorios
	home.pai = &pcUsuario;
	log.pai = &pcUsuario;
	bin.pai = &pcUsuario;
	sys.pai = &pcUsuario;
	lib.pai = &pcUsuario;

	// Principais
	pcUsuario.subdiretorios["home"] = home;
	pcUsuario.subdiretorios["bin"] = bin;
	pcUsuario.subdiretorios["sys"] = sys;
	pcUsuario.subdiretorios["lib"] = lib;
	pcUsuario.subdiretorios["log"] = log;
	pcUsuario.pai = &pcUsuario;

	// Definindo server e diretorio iniciais
	_server = pcUsuario;
	_dirAtual = home;
	
	// iniciando programa
	cout << initMsg << endl;
	cout << email1 << endl;
	cout << pressEnter << endl;
	
	string comando;
	getline(cin, comando);
	
	loopSistema();
	// falta o arquivo de ameaça iremos_te_pegar.txt
	return 1;
}
