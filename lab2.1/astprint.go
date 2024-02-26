package main

import (
	"fmt"
	"go/ast"
	"go/format"
	"go/parser"
	"go/token"
	"os"
)

func insertImport(file *ast.File, importName string) {
	ast.Inspect(file, func(node ast.Node) bool {
		if File, ok := node.(*ast.File); ok {
			File.Decls = append(
				append([]ast.Decl{},
					&ast.GenDecl{
						Doc: nil,
						Tok: token.IMPORT,
						Specs: []ast.Spec{
							&ast.ImportSpec{
								Doc:  nil,
								Name: nil,
								Path: &ast.BasicLit{
									Kind:  token.STRING,
									Value: "\"" + importName + "\"",
								},
								Comment: nil,
							},
						},
					}),
				File.Decls...,
			)
		}
		return true
	})
}

// поискать либу для генерации UUID (уникального id)
func insertTimerSet(file *ast.File) {
	ast.Inspect(file, func(node ast.Node) bool {
		if File, ok := node.(*ast.File); ok {
			File.Decls = append(
				append([]ast.Decl{},
					&ast.GenDecl{
						Doc: nil,
						Tok: token.VAR,
						Specs: []ast.Spec{
							&ast.ValueSpec{
								Doc: nil,
								Names: []*ast.Ident{
									{
										Name: "____NOW____",
										Obj: &ast.Object{
											Kind: ast.ObjKind(token.VAR),
											Name: "____NOW____",
											Data: 0,
											Type: nil,
										},
									},
								},
								Type: nil,
								Values: []ast.Expr{
									&ast.CallExpr{
										Fun: &ast.SelectorExpr{
											X: &ast.Ident{
												Name: "time",
												Obj:  nil,
											},
											Sel: &ast.Ident{
												Name: "Now",
												Obj:  nil,
											},
										},
										Args: nil,
									},
								},
							},
						},
					}),
				File.Decls...,
			)
		}
		return true
	})
}

func insertFuncTimer(file *ast.File) {
	ast.Inspect(file, func(node ast.Node) bool {
		if FuncDecl, ok := node.(*ast.FuncDecl); ok {
			generateCallExprNode := func(startEnd string) *ast.CallExpr {
				return &ast.CallExpr{
					Fun: &ast.SelectorExpr{
						X: &ast.Ident{
							Name: "fmt",
							Obj:  nil,
						},
						Sel: &ast.Ident{
							Name: "Printf",
							Obj:  nil,
						},
					},
					Args: []ast.Expr{
						&ast.BasicLit{
							Kind:  token.STRING,
							Value: "\"" + startEnd + " %s %s\\n\"",
						},
						&ast.BasicLit{
							Kind:  token.STRING,
							Value: "\"" + FuncDecl.Name.Name + "\"",
						},
						&ast.CallExpr{
							Fun: &ast.SelectorExpr{
								X: &ast.CallExpr{
									Fun: &ast.SelectorExpr{
										X: &ast.Ident{
											Name: "time",
											Obj:  nil,
										},
										Sel: &ast.Ident{
											Name: "Since",
											Obj:  nil,
										},
									},
									Args: []ast.Expr{
										&ast.Ident{
											Name: "____NOW____",
											Obj:  nil,
										},
									},
								},
								Sel: &ast.Ident{
									Name: "String",
									Obj:  nil,
								},
							},
							Args: nil,
						},
					},
				}
			}
			FuncDecl.Body.List = append(
				append(append([]ast.Stmt{},
					&ast.ExprStmt{
						X: generateCallExprNode("Starts"),
					},
				),
					&ast.DeferStmt{
						Call: generateCallExprNode("Ends"),
					},
				),
				FuncDecl.Body.List...)
		}
		return true
	})
}

func main() {
	if len(os.Args) != 2 {
		fmt.Printf("usage: astprint <filename.go>\n")
		return
	}

	// Создаём хранилище данных об исходных файлах
	fset := token.NewFileSet()

	// Вызываем парсер
	if file, err := parser.ParseFile(
		fset,                 // данные об исходниках
		os.Args[1],           // имя файла с исходником программы
		nil,                  // пусть парсер сам загрузит исходник
		parser.ParseComments, // приказываем сохранять комментарии
	); err == nil {
		insertTimerSet(file)
		insertImport(file, "fmt")
		insertImport(file, "time")
		insertFuncTimer(file)

		if format.Node(os.Stdout, fset, file) != nil {
			fmt.Printf("Formatter error: %v\n", err)
		}

		// Если парсер отработал без ошибок, печатаем дерево
		ast.Fprint(os.Stdout, fset, file, nil)
	} else {
		// в противном случае, выводим сообщение об ошибке
		fmt.Printf("Error: %v", err)
	}
}
